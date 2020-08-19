#pragma once

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <threads.h>
#include <stdlib.h>

#include <scheduler/fiber.h>
#include <structures/list.h>
#include <locks/atomics.h>

typedef struct scheduler
{
    size_t threads;
    list **queues;
    list *threads_pool;
    int threads_running;
    int terminate;
    size_t count;
    spinlock lock_spinlock;
} scheduler;

extern thread_local scheduler *current_scheduler;

scheduler *new_default_scheduler(void);
scheduler *new_scheduler(unsigned int using_threads);

void run_scheduler(scheduler* sched);

fiber *submit(fiber_routine routine, void *args);
fiber *spawn(scheduler *sched, fiber_routine routine, void *args);
void join(fiber *fib);
void yield(void);
void terminate_scheduler(scheduler* sched);
void shutdown(scheduler* sched);
void sleep_for(unsigned long duration);