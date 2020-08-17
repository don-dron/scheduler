#pragma once

#include <scheduler/scheduler.h>

struct thread_node;
typedef struct thread_node thread_node;
struct thread_node
{
    list_node lst_node;
    pthread_t thr;
};

struct fiber_node;
typedef struct fiber_node fiber_node;
struct fiber_node
{
    list_node lst_node;
    fiber *fib;
};

void return_to_queue(fiber_node *node, int thread_index)
{
    list_push_back(current_scheduler->queues[(thread_index + current_scheduler->threads - 1) % current_scheduler->threads], (list_node *)node);
}

void insert_to_minimum_queue(fiber *fib)
{
    int index = 0;
    int threads = current_scheduler->threads;

    for (int i = 0; i < threads; ++i)
    {
        if (current_scheduler->queues[i]->size < current_scheduler->queues[index]->size)
        {
            index = i;
        }
    }

    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    list_push_back(current_scheduler->queues[index], (list_node *)fib_node);
}

void run_task(fiber *routine)
{
    current_fiber = routine;
    current_fiber->state = running;
    switch_context(&current_fiber->external_context, &current_fiber->context);

    if (current_fiber->state != terminated)
    {
        insert_to_minimum_queue(current_fiber);
    }
    else
    {
        free_fiber(current_fiber);
        __atomic_fetch_sub(&current_scheduler->count, 1, __ATOMIC_SEQ_CST);
    }
}

void thread_cycle(void *arg)
{
    int t = ((int *)arg)[0];
    free(arg);
    list *queue = current_scheduler->queues[t];

    while (!current_scheduler->threads_run)
    {
        usleep(1);
    }

    while (1)
    {
        __atomic_fetch_add(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
        fiber_node *fib_node = (fiber_node *)list_pop_front(queue);

        if (fib_node && fib_node->fib->state == sleeping)
        {
            fiber *fib = fib_node->fib;
            if (fib->wakeup > time(0))
            {
                free(fib_node);
                insert_to_minimum_queue(fib);
                __atomic_fetch_sub(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
                continue;
            }
        }

        if (fib_node)
        {
            fiber *fib = fib_node->fib;
            free(fib_node);
            run_task(fib);
            __atomic_fetch_sub(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
        }
        else
        {
            if (current_scheduler->end_threads)
            {
                __atomic_fetch_sub(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
                return;
            }
            else
            {
                int threads = current_scheduler->threads;
                int current = t;

                fiber_node *stolen = 0;
                while (1)
                {
                    current += 1;
                    current %= threads;
                    stolen = (fiber_node *)list_pop_back(queue);
                    if (stolen != 0 || current == t)
                    {
                        break;
                    }
                }

                if (stolen)
                {
                    fiber *fib = stolen->fib;
                    free(stolen);
                    run_task(fib);
                    __atomic_fetch_sub(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
                }
                else
                {
                    __atomic_fetch_sub(&current_scheduler->task_now, 1, __ATOMIC_SEQ_CST);
                    usleep(2);
                }
            }
        }
    }
}

void join(fiber *fib)
{
    while (fib->state != terminated)
    {
        yield();
    }
}

void new_scheduler()
{
    size_t threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    if (current_scheduler != 0)
    {
        abort();
    }

    current_scheduler = (scheduler *)malloc(sizeof(scheduler));

    current_scheduler->threads_pool = create_list();
    current_scheduler->queues = (list **)calloc(sizeof(list *), threads);
    current_scheduler->end_threads = 0;
    current_scheduler->threads_run = 0;
    current_scheduler->threads = threads;

    for (int t = 0; t < threads; t++)
    {
        current_scheduler->queues[t] = create_list();
    }

    for (int t = 0; t < threads; t++)
    {
        thread_node *thr_node = (thread_node *)malloc(sizeof(thread_node));

        int *args = (int *)malloc(sizeof(int) * 1);
        args[0] = t;

        pthread_create(&thr_node->thr, 0, thread_cycle, (void *)args);

        list_push_back(current_scheduler->threads_pool, (list_node *)thr_node);
    }
}

void run_scheduler()
{
    current_scheduler->threads_run = 1;
}

void insert_fiber(fiber *fib)
{
    __atomic_fetch_add(&current_scheduler->count, 1, __ATOMIC_SEQ_CST);
    insert_to_minimum_queue(fib);
}

fiber *spawn(fiber_routine routine)
{
    fiber *fib = create_fiber(routine);
    insert_fiber(fib);
    return fib;
}

void switch_to_scheduler(fiber *fib)
{
    fiber *next = fib;

    while (next->parent)
    {
        next = next->parent;
    }

    switch_context(&fib->context, &next->external_context);
}

void sleep_for(unsigned long duration)
{
    fiber *fib = current_fiber;
    fib->state = sleeping;
    fib->wakeup = time(0) + duration;
    switch_context(&current_fiber->context, &current_fiber->external_context);
}

fiber *submit(fiber_routine routine)
{
    fiber *fib = create_fiber(routine);
    fib->external_context = current_fiber->context;
    insert_fiber(fib);
    return fib;
}

void terminate_scheduler()
{
    shutdown();
    current_scheduler->end_threads = 1;

    while (1)
    {
        thread_node *thr_node = (thread_node *)list_pop_front(current_scheduler->threads_pool);

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

    free(current_scheduler->threads_pool);

    for (int i = 0; i < current_scheduler->threads; i++)
    {
        free(current_scheduler->queues[i]);
    }

    free(current_scheduler->queues);

    free(current_scheduler);
    current_scheduler = 0;
}

void shutdown()
{
    while (current_scheduler->count || current_scheduler->task_now)
    {
        usleep(2);
    }
}

void yield()
{
    current_fiber->state = runnable;
    switch_context(&current_fiber->context, &current_fiber->external_context);
}