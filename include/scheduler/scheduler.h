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
    int threads_run;
    int end_threads;
    size_t count;
    size_t task_now;
    spinlock lock_spinlock;
} scheduler;

static scheduler *current_scheduler;
static thread_local size_t number;

void new_scheduler();
void run_scheduler();

fiber* submit(fiber_routine routine);
fiber* spawn(fiber_routine routine);
void join(fiber* fib);
void yield();
void terminate_scheduler();
void shutdown();
void sleep_for(unsigned long duration);