#pragma once

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <threads.h>
#include <stdlib.h>

#include <scheduler/fiber.h>
#include <structures/list.h>

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

static thread_local scheduler *current_scheduler;
static thread_local size_t number;

scheduler *new_default_scheduler();
scheduler *new_scheduler(unsigned int using_threads);

void run_scheduler();

fiber *submit(fiber_routine routine);
fiber *spawn(scheduler *sched, fiber_routine routine);
void join(fiber *fib);
void yield();
void terminate_scheduler();
void shutdown();
void sleep_for(unsigned long duration);