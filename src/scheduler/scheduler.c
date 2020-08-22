#include <scheduler/scheduler.h>

#define INTERVAL 20

thread_local unsigned long pid = 22;
thread_local scheduler *current_scheduler = NULL;

typedef struct fiber_node
{
    list_node lst_node;
    fiber *fib;
} fiber_node;

static void insert_to_minimum_queue(scheduler *sched, fiber *fib)
{
    size_t index = 0;
    size_t threads = sched->threads;

    for (size_t i = 0; i < threads; ++i)
    {
        if (sched->queues[i]->size < sched->queues[index]->size)
        {
            index = i;
        }
    }

    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    list_push_back(sched->queues[index], (list_node *)fib_node);
}

static long sub_time(struct timespec *t1, struct timespec *t2)
{
    long delta = 1000 * 1000 * (t1->tv_sec - t2->tv_sec) + (t1->tv_nsec - t2->tv_nsec) / 1000;
    return delta;
}

static void run_task(fiber *routine, unsigned long thread_number)
{
    lock_spinlock(&routine->lock);

    current_fiber = routine;
    fiber *temp = current_fiber;

    if (current_fiber->state == sleeping)
    {
        struct timespec timer;
        clock_gettime(CLOCK_REALTIME, &timer);

        if (sub_time(&current_fiber->wakeup, &timer) > 0)
        {
            insert_to_minimum_queue(current_scheduler, current_fiber);
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
        clock_gettime(CLOCK_REALTIME, &current_fiber->start);
        switch_context(&current_fiber->external_context, &current_fiber->context);

#if DEBUG
        printf("[OUT_FIBER] Out fiber %ld %ld %d\n", thread_number, current_fiber->id, current_fiber->state);
#endif

        if (current_fiber->state != terminated)
        {
            insert_to_minimum_queue(current_scheduler, current_fiber);
            current_fiber = NULL;
            unlock_spinlock(&temp->lock);
        }
        else
        {
            current_fiber = NULL;
            unlock_spinlock(&temp->lock);
            free_fiber(temp);
            inc((unsigned long *)&current_scheduler->end_count);
        }
    }
    else
    {
        printf("[ERROR] Run task wrong state  %d\n", current_fiber->state);
        exit(1);
    }
}

static void scheduler_pause()
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

static fiber_node *steal_task(list *queue, unsigned long thread_number)
{
    fiber_node *stolen = NULL;
    size_t threads = current_scheduler->threads;
    unsigned long current = thread_number;
    while (1)
    {
        current += 1;
        current %= threads;
        stolen = (fiber_node *)list_pop_back(queue);
        if (stolen != 0 || current == thread_number)
        {
            return stolen;
        }
    }
}

static void schedule(unsigned long thread_number)
{
    list *queue = current_scheduler->queues[thread_number];
    while (1)
    {
        scheduler_pause();

        if (current_scheduler->terminate)
        {
            return;
        }

        fiber_node *fib_node = (fiber_node *)list_pop_front(queue);

        if (fib_node)
        {
            fiber *fib = fib_node->fib;
            free(fib_node);
            run_task(fib, thread_number);
        }
        else
        {
            if (current_scheduler->terminate)
            {
                return;
            }
            else
            {
                fiber_node *stolen = steal_task(queue, thread_number);

                if (stolen)
                {
                    fiber *fib = stolen->fib;
                    free(stolen);
                    run_task(fib, thread_number);
                }
                else
                {
                    usleep(2);
                }
            }
        }
    }
}

static void insert_fiber(scheduler *sched, fiber *fib)
{
    inc((unsigned long *)&sched->count);
    insert_to_minimum_queue(sched, fib);
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

        // usleep(INTERVAL);

        for (size_t i = 0; i < current_scheduler->threads; i++)
        {
            pthread_kill(current_scheduler->threads_pool[i], SIGVTALRM);
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
                return;
            }
            struct timespec current_time;
            clock_gettime(CLOCK_REALTIME, &current_time);
            long delta = sub_time(&current_time, &temp->start);
            if (delta > 500)
            {
                if (temp->state == running)
                {
#if DEBUG
                    printf("[INTERRUPT] Interrupt fiber in scheduler thread with tid = %ld  with id = %ld  with last slice %ld delta\n", pid, temp->id, delta);
#endif
                    inc(&interrupt_count);
                    temp->state = runnable;
                    switch_context(&temp->context, &temp->external_context);
                    unlock_spinlock(&temp->lock);
                }
                else
                {
                    printf("[ERROR] Handler wrong state: in scheduler thread with tid = %ld  with id = %ld  with state = %d \n", pid, temp->id, temp->state);
                    exit(1);
                }
            }
            else
            {
                unlock_spinlock(&temp->lock);
            }
        }
    }
}

static void *run_fibers_handler(void *arg)
{
    unsigned long thread_number = ((unsigned long *)arg)[0];
    pid = thread_number;
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
    size_t threads = using_threads;

    sched->threads_pool = (pthread_t *)malloc(sizeof(pthread_t) * threads);
    sched->queues = (list **)calloc(sizeof(list *), threads);
    sched->count = 0;
    sched->end_count = 0;
    sched->terminate = 0;
    sched->threads_running = 0;
    sched->threads = threads;
    sched->current_fibers = (fiber ***)malloc(sizeof(fiber) * threads);

    memset(&sched->sigact, 0, sizeof(sched->sigact));
    sched->sigact.sa_handler = handler;
    sigaddset(&(sched->sigact.sa_mask), SIGVTALRM);
    sigaction(SIGVTALRM, &sched->sigact, 0);

    for (unsigned long index = 0; index < threads; index++)
    {
        sched->queues[index] = (list *)malloc(sizeof(list));
        create_list(sched->queues[index]);
    }

    pthread_create(&sched->main_thread, 0, main_thread_func, sched);

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

    clock_gettime(CLOCK_REALTIME, &temp->wakeup);
    temp->wakeup.tv_nsec += (long)(duration * 1000);

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

void join(fiber *fib)
{
    while (1)
    {
        if (fib->state != terminated)
        {
            if (current_fiber)
            {
                yield();
            }
            else
            {
                usleep(2);
            }
        }
        else
        {
            break;
        }
    }
}

fiber *submit(fiber_routine routine, void *args)
{
    if (current_fiber == NULL)
    {
        return NULL;
    }

    fiber *fib = create_fiber(routine, args);
    fib->external_context = current_fiber->context;
    insert_fiber(current_scheduler, fib);
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

    for (size_t i = 0; i < sched->threads; i++)
    {
        free(sched->queues[i]);
    }

    free(sched->queues);

    return 0;
}

void shutdown(scheduler *sched)
{
    while (sched->count != sched->end_count)
    {
        usleep(2);
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