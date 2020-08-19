#include <scheduler/scheduler.h>

typedef struct thread_node
{
    list_node lst_node;
    pthread_t thr;
} thread_node;

typedef struct fiber_node
{
    list_node lst_node;
    fiber *fib;
} fiber_node;

thread_local scheduler* current_scheduler = NULL;

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

static void run_task(fiber *routine)
{
    current_fiber = routine;
    current_fiber->state = running;
    switch_context(&current_fiber->external_context, &current_fiber->context);

    if (current_fiber->state != terminated)
    {
        insert_to_minimum_queue(current_scheduler, current_fiber);
    }
    else
    {
        free_fiber(current_fiber);
        dec((unsigned long*)&current_scheduler->count);
    }
}

static int more(struct timespec *t1, struct timespec *t2)
{
    long delta = 1000 * 1000 * 1000 * (t1->tv_sec - t2->tv_sec) + (t1->tv_nsec - t2->tv_nsec);
    return delta > 0;
}

static void schedule(unsigned long thread_number)
{
    list *queue = current_scheduler->queues[thread_number];
    while (1)
    {
        while (!current_scheduler->threads_running)
        {
            if (current_scheduler->terminate)
            {
                return;
            }

            usleep(1);
        }

        fiber_node *fib_node = (fiber_node *)list_pop_front(queue);

        if (fib_node && fib_node->fib->state == sleeping)
        {
            fiber *fib = fib_node->fib;
            struct timespec timer;
            clock_gettime(CLOCK_REALTIME, &timer);

            if (more(&fib->wakeup, &timer))
            {
                free(fib_node);
                insert_to_minimum_queue(current_scheduler, fib);
                continue;
            }
        }

        if (fib_node)
        {
            fiber *fib = fib_node->fib;
            free(fib_node);
            run_task(fib);
        }
        else
        {
            if (current_scheduler->terminate)
            {
                return;
            }
            else
            {
                size_t threads = current_scheduler->threads;
                unsigned long current = thread_number;

                fiber_node *stolen = 0;
                while (1)
                {
                    current += 1;
                    current %= threads;
                    stolen = (fiber_node *)list_pop_back(queue);
                    if (stolen != 0 || current == thread_number)
                    {
                        break;
                    }
                }

                if (stolen)
                {
                    fiber *fib = stolen->fib;
                    free(stolen);
                    run_task(fib);
                }
                else
                {
                    usleep(2);
                }
            }
        }
    }
}

static void* thread_cycle(void *arg)
{
    unsigned long thread_number = ((unsigned long *)arg)[0];
    scheduler *sched = (scheduler *)((unsigned long *)arg)[1];

    current_scheduler = sched;

    free(arg);
    schedule(thread_number);
    return NULL;
}

static void insert_fiber(scheduler *sched, fiber *fib)
{
    inc((unsigned long*)&sched->count);
    insert_to_minimum_queue(sched, fib);
}

void join(fiber *fib)
{
    while (fib->state != terminated)
    {
        yield();
    }
}

scheduler *new_default_scheduler()
{
    return new_scheduler((unsigned int)sysconf(_SC_NPROCESSORS_ONLN) - 1);
}

scheduler *new_scheduler(unsigned int using_threads)
{
    size_t threads = using_threads;

    scheduler *sched = (scheduler *)malloc(sizeof(scheduler));

    sched->threads_pool = create_list();
    sched->queues = (list **)calloc(sizeof(list *), threads);
    sched->terminate = 0;
    sched->threads_running = 0;
    sched->threads = threads;

    for (unsigned long index = 0; index < threads; index++)
    {
        sched->queues[index] = create_list();
    }

    for (unsigned long index = 0; index < threads; index++)
    {
        thread_node *thr_node = (thread_node *)malloc(sizeof(thread_node));

        unsigned long *args = (unsigned long *)malloc(sizeof(unsigned long) * 1);
        args[0] = index;
        args[1] = (unsigned long)sched;

        pthread_create(&thr_node->thr, 0, thread_cycle, (void *)args);

        list_push_back(sched->threads_pool, (list_node *)thr_node);
    }

    return sched;
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
    fiber *fib = current_fiber;
    fib->state = sleeping;

    struct timespec timer;
    clock_gettime(CLOCK_REALTIME, &timer);
    timer.tv_nsec += (long)(duration * 1000);
    fib->wakeup = timer;

    switch_context(&current_fiber->context, &current_fiber->external_context);
}

fiber *submit(fiber_routine routine, void *args)
{
    fiber *fib = create_fiber(routine, args);
    fib->external_context = current_fiber->context;
    insert_fiber(current_scheduler, fib);
    return fib;
}

void terminate_scheduler(scheduler *sched)
{
    shutdown(sched);
    sched->terminate = 1;

    while (1)
    {
        thread_node *thr_node = (thread_node *)list_pop_front(sched->threads_pool);

        if (thr_node == 0)
        {
            break;
        }
        else
        {
            pthread_join(thr_node->thr, NULL);
            free(thr_node);
        }
    }

    free(sched->threads_pool);

    for (size_t i = 0; i < sched->threads; i++)
    {
        free(sched->queues[i]);
    }

    free(sched->queues);

    free(sched);
}

void shutdown(scheduler *sched)
{
    while (sched->count)
    {
        usleep(2);
    }
    sched->threads_running = 0;
}

void yield()
{
    current_fiber->state = runnable;
    switch_context(&current_fiber->context, &current_fiber->external_context);
}