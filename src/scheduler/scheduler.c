#include <scheduler/scheduler.h>

#define INTERVAL 1000

thread_local unsigned long thread_id = 22;
thread_local scheduler *current_scheduler = NULL;

//
//    Handler threads - pthreads . Each pthread gets task - fiber from pull and run him.
//    After yield,sleep,terminate - cooperative methods or interrupt - preemption methods
//    handler returns for scheduling.
//
//    Detail for one thread:
//
//              scheduling          scheduling .etc
//         (gets fiber from pull                                              scheduler
//              and run him)                                                   _______
//              _______           __________ .....                            ^       |
//              ^      |          ^                                           |       |
//              |      V          |                                           |       V
//  -after run----------*---------*--------------------*----------------------------------------------------->  work time
//                      |         ^                    |  work in          ^          |                   |
//                      V_________|  yield()           |  fiber is long    |          |                   ^
//                       work in     sleep_for()       V--------           |          |                   |
//   useful work *-----*    fiber    `end of fiber`            |           |          |                   |
//                      |         |                                        |          |                   ^ returns to fiber context
//                                                             |           |          |                   |         for handler context
//                      |         |                                        |          V                   |
//                                                             |   switch context    switch context       |
//                      |         |                                 to scheduler      to signal handler   |
//                      *         *                            |           ^          |                   |
//          switch to fiber      switch to scheduler                       |          |                   |
//                                                             |           |          |                   |
//                                                                    signal handler  |**************** > |
//            cooperative mechanizm                            |           |                         ^
//                                                                         |                         | remove additional stack frame
//                                                             |           |                         |          for handler
//                                              it's kernel                ^                        KERNEL
//                                                work         |           |
//                                      |******************* >             |
//                                      |                      |         new stack frame(created by kernel)
//                                      |                      ******* > for handler(in userspace) - we have additional
//                                      |                                stack frame above fiber frame
//                                      |
//
//                                   KERNEL **************** < *********   ^
//                                                                         |
//                                                                 send signal from additional thread
//

static inline long clock_to_microseconds(long time)
{
    // Magic constant
    return time * 4;
}

static inline long sub_time(clock_t end, clock_t start)
{
    // Returns time delta
    long delta = clock_to_microseconds((long)(end - start));
    return delta;
}

static void run_task(fiber *routine)
{
    // If not locking, a context switch can be called during another
    // context switch in the same thread - it's ub and crushed stack frame
    // This lock will unlocked in fiber body.
    lock_spinlock(&routine->lock);

    current_fiber = routine;
    fiber *temp = current_fiber;

    if (current_fiber->state == sleeping)
    {
        // Checks time for wake up
        if (sub_time(current_fiber->wakeup, clock()) > 0)
        {
            // Wake up failed, sleep yet
            return_to_pull(current_scheduler, current_fiber);
            current_fiber = NULL;
            unlock_spinlock(&temp->lock);
            return;
        }
        else
        {
            // Wake up success, run
            current_fiber->state = runnable;
        }
    }

    if (current_fiber->state == starting || current_fiber->state == runnable)
    {
#if DEBUG
        printf("[IN_FIBER ] To fiber %ld %ld %d\n", thread_number, current_fiber->id, current_fiber->state);
#endif

        current_fiber->state = running;

        // Save start fiber time
        current_fiber->start = clock();

        // Returns to fiber
        //
        // * ~ Magic ~ *
        //
        switch_context(&current_fiber->external_context, &current_fiber->context);
        //
        // Returns to scheduler
#if DEBUG
        printf("[OUT_FIBER] Out fiber %ld %ld %d\n", thread_number, current_fiber->id, current_fiber->state);
#endif

        // If not terminated - returns to pull
        if (current_fiber->state != terminated)
        {
            // Returns to pull
            return_to_pull(current_scheduler, current_fiber);
            current_fiber = NULL;

            // This unlock is unlocked fiber body lock
            unlock_spinlock(&temp->lock);
        }
        else
        {
            current_fiber = NULL;
            inc((unsigned long *)&current_scheduler->end_count);

            // This unlock is unlocked fiber body lock
            unlock_spinlock(&temp->lock);

            // Fiber delete
            free_fiber(temp);
        }
    }
    else
    {
        printf("[ERROR] Run task wrong state  %d\n", current_fiber->state);
        exit(1);
    }
}

static inline void scheduler_pause()
{
    // Loop if not running and go out if terminate
    while (!current_scheduler->threads_running)
    {
        if (current_scheduler->terminate)
        {
            return;
        }

        usleep(10);
    }
}

static void schedule()
{
    while (1)
    {
        // If scheduler not run - block
        scheduler_pause();

        if (current_scheduler->terminate)
        {
            return;
        }

        // Gets fiber from pull
        fiber *fib = get_from_pool();

        if (fib)
        {
            // Run task
            run_task(fib);
        }
        else
        {
            // Sleep if work queue is empty
            usleep(100);
        }
    }
}

static void insert_fiber(scheduler *sched, fiber *fib)
{
    inc((unsigned long *)&sched->count);
    return_to_pull(sched, fib);
}

#if INTERRUPT_ENABLED

static void *signal_thread_func(void *args)
{
    current_scheduler = (scheduler *)args;
    for (; !current_scheduler->terminate;)
    {
        // If scheduler not run - block
        scheduler_pause();

        if (current_scheduler->terminate)
        {
            return NULL;
        }

        // Wake up after interval
        usleep(INTERVAL);

        for (size_t i = 0; i < current_scheduler->threads; i++)
        {
            // Send signals to handlers
            pthread_kill(current_scheduler->threads_pool[i], SIGALRM);
        }
    }

    return NULL;
}

#endif

static void handler(int signo)
{
    // Handler for SIGALARM signals
    fiber *temp = current_fiber;

    if (temp)
    {
        // Try lock. If locked - switch context was be started - we can't handle signal.
        // If fiber was been interrupted - lock will be unlocked in fiber body.
        if (try_lock_spinlock(&temp->lock))
        {
            // Checks correctly fiber save
            if (current_fiber != temp)
            {
                unlock_spinlock(&temp->lock);
                inc(&interrupt_failed_count);
                return;
            }

            // Checks fiber time slice, if time slice is long fiber was been interrupted
            long delta = sub_time(clock(), temp->start);
            if (delta > 100)
            {
                if (temp->state == running)
                {
#if DEBUG
                    printf("[INTERRUPT] Interrupt fiber in scheduler thread with tid = %ld  with id = %ld  with last slice %ld delta\n", thread_id, temp->id, delta);
#endif
                    inc(&interrupt_count);
                    temp->state = runnable;

                    // Interrupt
                    switch_context(&temp->context, &temp->external_context);

                    // Unlock fiber body lock
                    unlock_spinlock(&temp->lock);
                }
                else
                {
                    printf("[ERROR] Handler wrong state: in scheduler thread with tid = %ld  with id = %ld  with state = %d \n", thread_id, temp->id, temp->state);
                    exit(1);
                }
            }
            else
            {
                unlock_spinlock(&temp->lock);
                inc(&interrupt_failed_count);
            }
        }
        else
        {
#if DEBUG
            printf("[WARNING] Fail spinlock lock\n");
#endif
            inc(&interrupt_failed_count);
        }
    }
    else
    {
#if DEBUG
        printf("[WARNING] Null fiber in %ld %ld\n", thread_id, pthread_self());
#endif
        inc(&interrupt_failed_count);
    }
}

static void *run_fibers_handler(void *arg)
{
    unsigned long thread_number = ((unsigned long *)arg)[0];
    scheduler *sched = (scheduler *)((unsigned long *)arg)[1];

    // Setting params to TLS

    thread_id = thread_number;
    current_scheduler = sched;
    current_scheduler->current_fibers[thread_number] = &current_fiber;

    free(arg);

    // Run schedule
    schedule(thread_number);
    return NULL;
}

int new_default_scheduler(scheduler *sched)
{
    return new_scheduler(sched, (unsigned int)sysconf(_SC_NPROCESSORS_ONLN) - 1);
}

int new_scheduler(scheduler *sched, unsigned int using_threads)
{
    // Init block
    size_t threads = using_threads;

    sched->threads_pool = (pthread_t *)malloc(sizeof(pthread_t) * threads);
    sched->count = 0;
    sched->end_count = 0;
    sched->terminate = 0;
    sched->threads_running = 0;
    sched->threads = threads;
    sched->current_fibers = (fiber ***)malloc(sizeof(fiber) * threads);

    // Sets signals parameters for interrupting fibers

    // mask SIGALRM in all threads by default
    sigemptyset(&(sched->sigact.sa_mask));
    sigaddset(&(sched->sigact.sa_mask), SIGALRM);

    // we need a signal handler.
    // The default is to call abort() and
    // setting SIG_IGN might cause the signal
    // to not be delivered at all.

    memset(&sched->sigact, 0, sizeof(sched->sigact));
    sched->sigact.sa_handler = handler;
    sigaction(SIGALRM, &sched->sigact, NULL);

    // Create manager for scheduler
    create_scheduler_manager(sched);

#if INTERRUPT_ENABLED
    // Create thread for sending signals to handlers-threads
    pthread_create(&sched->signal_thread, 0, signal_thread_func, sched);
    int policy = SCHED_OTHER;
    struct sched_param param;

    pthread_getschedparam(sched->signal_thread, &policy, &param);
    param.sched_priority = 99;
    pthread_setschedparam(sched->signal_thread, policy, &param);
#endif

    // Create handlers-threads
    for (unsigned long index = 0; index < threads; index++)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        unsigned long *args = (unsigned long *)malloc(sizeof(unsigned long) * 2);
        args[0] = index;
        args[1] = (unsigned long)sched;

        pthread_create(&sched->threads_pool[index], &attr, run_fibers_handler, (void *)args);
    }

    return 0;
}

void run_scheduler(scheduler *sched)
{
    sched->threads_running = 1;
}

fiber *spawn(scheduler *sched, fiber_routine routine, void *args)
{
    fiber *fib = create_fiber(routine, args);
    fib->sched = sched;
    insert_fiber(sched, fib);
    return fib;
}

void sleep_for(unsigned long duration)
{
    if (!current_fiber)
    {
        printf("[ERROR] Out of fiber\n");
        exit(1);
    }

    // This lock will be unlocked in run_task function after switch context

    lock_spinlock(&current_fiber->lock);

    fiber *temp = current_fiber;

    // Sets time for wake up
    temp->wakeup = clock();
    temp->wakeup += clock_to_microseconds((long)duration);

    if (temp->state == running)
    {
        temp->state = sleeping;

        // Return to run_task function
        switch_context(&temp->context, &temp->external_context);

        // This unlock unlocking lock locked in run_task function
        unlock_spinlock(&temp->lock);
    }
    else
    {
        printf("[ERROR] Sleep wrong state %d\n", temp->state);
        exit(1);
    }
}

fiber *submit(fiber_routine routine, void *args)
{
    if (current_fiber == NULL)
    {
        printf("[ERROR] Submit out of fiber\n");
        exit(1);
    }

    // Lock for create new fiber - block context switching
    lock_spinlock(&current_fiber->lock);
    fiber *fib = create_fiber(routine, args);
    fib->external_context = current_fiber->context;
    fib->sched = current_scheduler;
    insert_fiber(current_scheduler, fib);
    unlock_spinlock(&current_fiber->lock);

    return fib;
}

int terminate_scheduler(scheduler *sched)
{
    // Shutdown before terminate
    shutdown(sched);

    sched->terminate = 1;

#if INTERRUPT_ENABLED
    // Join threads
    pthread_join(sched->signal_thread, NULL);
#endif

    for (size_t i = 0; i < sched->threads; i++)
    {
        pthread_join(sched->threads_pool[i], NULL);
    }

    free(sched->threads_pool);

    // Scheduler manager teminate
    free_scheduler_manager(sched);

    return 0;
}

void join(fiber *fib)
{
    while (1)
    {
        if (fib->state != terminated)
        {
            if (current_fiber)
            {
                // If we in fiber
                sleep_for(200);
            }
            else
            {
                // If we not in fiber
                usleep(200);
            }
        }
        else
        {
            break;
        }
    }
}

void shutdown(scheduler *sched)
{
    while (sched->count != sched->end_count)
    {
        // Sleep if failed
        usleep(200);
    }
    sched->threads_running = 0;
}

void yield()
{
    // This lock will be unlocked in run_task function after switch context
    lock_spinlock(&current_fiber->lock);
    fiber *temp = current_fiber;

    if (temp->state == running)
    {
        temp->state = runnable;

        // Returns to run_task function
        switch_context(&temp->context, &temp->external_context);

        // This unlock unlock lock locked in run_task function before switch context
        unlock_spinlock(&temp->lock);
    }
    else
    {
        printf("[ERROR] Yield wrong state %d\n", temp->state);
        exit(1);
    }
}