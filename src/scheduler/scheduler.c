#include <scheduler/scheduler.h>

#define INTERVAL 1000

thread_local unsigned long thread_id = 22;
thread_local scheduler *current_scheduler = NULL;

static inline long clock_to_microseconds(long time)
{
    return time * 4;
}

static inline long sub_time(clock_t end, clock_t start)
{
    long delta = clock_to_microseconds((long)(end - start));
    return delta;
}

static void run_task(fiber *routine)
{
    lock_spinlock(&routine->lock);

    current_fiber = routine;
    fiber *temp = current_fiber;

    if (current_fiber->state == sleeping)
    {
        if (sub_time(current_fiber->wakeup, clock()) > 0)
        {
            return_to_pull(current_scheduler, current_fiber);
            current_fiber = NULL;
            unlock_spinlock(&temp->lock);
            return;
        }
        else
        {
            current_fiber->state = runnable;
        }
    }

    if (current_fiber->state == starting || current_fiber->state == runnable)
    {
#if DEBUG
        printf("[IN_FIBER ] To fiber %ld %ld %d\n", thread_number, current_fiber->id, current_fiber->state);
#endif

        current_fiber->state = running;
        current_fiber->start = clock();

        switch_context(&current_fiber->external_context, &current_fiber->context);

#if DEBUG
        printf("[OUT_FIBER] Out fiber %ld %ld %d\n", thread_number, current_fiber->id, current_fiber->state);
#endif

        if (current_fiber->state != terminated)
        {
            return_to_pull(current_scheduler, current_fiber);
            current_fiber = NULL;
            unlock_spinlock(&temp->lock);
        }
        else
        {
            current_fiber = NULL;
            inc((unsigned long *)&current_scheduler->end_count);
            unlock_spinlock(&temp->lock);
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
        scheduler_pause();

        if (current_scheduler->terminate)
        {
            return;
        }

        fiber *fib = get_from_pool();

        if (fib)
        {
            run_task(fib);
        }
        else if (current_scheduler->terminate)
        {
            return;
        }
        else
        {
            usleep(100);
        }
    }
}

static void insert_fiber(scheduler *sched, fiber *fib)
{
    inc((unsigned long *)&sched->count);
    return_to_pull(sched, fib);
}

static void *main_thread_func(void *args)
{
    current_scheduler = (scheduler *)args;
    for (; !current_scheduler->terminate;)
    {
        scheduler_pause();

        if (current_scheduler->terminate)
        {
            return NULL;
        }

        usleep(INTERVAL);

        for (size_t i = 0; i < current_scheduler->threads; i++)
        {
            pthread_kill(current_scheduler->threads_pool[i], SIGALRM);
        }
    }

    return NULL;
}

static void handler(int signo)
{
    fiber *temp = current_fiber;
    if (temp)
    {
        if (try_lock_spinlock(&temp->lock))
        {
            if (current_fiber != temp)
            {
                unlock_spinlock(&temp->lock);
                inc(&interrupt_failed_count);
                return;
            }

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
                    switch_context(&temp->context, &temp->external_context);
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
    thread_id = thread_number;
    scheduler *sched = (scheduler *)((unsigned long *)arg)[1];

    current_scheduler = sched;
    current_scheduler->current_fibers[thread_number] = &current_fiber;

    free(arg);

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

    /* mask SIGALRM in all threads by default */
    sigemptyset(&(sched->sigact.sa_mask));
    sigaddset(&(sched->sigact.sa_mask), SIGALRM);

    /* we need a signal handler.
     * The default is to call abort() and 
     * setting SIG_IGN might cause the signal
     * to not be delivered at all.
     **/
    memset(&sched->sigact, 0, sizeof(sched->sigact));
    sched->sigact.sa_handler = handler;
    sigaction(SIGALRM, &sched->sigact, NULL);
    
    // Create manager for scheduler
    create_scheduler_manager(sched);

    // Create thread for sending signals to handlers-threads
    pthread_create(&sched->main_thread, 0, main_thread_func, sched);
    int policy = SCHED_OTHER;
    struct sched_param param;

    pthread_getschedparam(sched->main_thread, &policy, &param);
    param.sched_priority = 99;
    pthread_setschedparam(sched->main_thread, policy, &param);

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

    lock_spinlock(&current_fiber->lock);

    fiber *temp = current_fiber;

    temp->wakeup = clock();
    temp->wakeup += clock_to_microseconds((long)duration);

    if (temp->state == running)
    {
        temp->state = sleeping;

        switch_context(&temp->context, &temp->external_context);
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
    shutdown(sched);

    sched->terminate = 1;

    pthread_join(sched->main_thread, NULL);

    for (size_t i = 0; i < sched->threads; i++)
    {
        pthread_join(sched->threads_pool[i], NULL);
    }

    free(sched->threads_pool);

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
                sleep_for(200);
            }
            else
            {
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
        usleep(200);
    }
    sched->threads_running = 0;
}

void yield()
{
    lock_spinlock(&current_fiber->lock);
    fiber *temp = current_fiber;

    if (temp->state == running)
    {
        temp->state = runnable;
        switch_context(&temp->context, &temp->external_context);
        unlock_spinlock(&temp->lock);
    }
    else
    {
        printf("[ERROR] Yield wrong state %d\n", temp->state);
        exit(1);
    }
}