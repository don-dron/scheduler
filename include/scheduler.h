#pragma once

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <threads.h>
#include <stdlib.h>

#include <fiber.h>
#include <list.h>

struct scheduler;
typedef struct scheduler scheduler;

struct scheduler
{
    size_t threads;
    list **queues;
    list *threads_pool;
    int threads_run;
    int end_threads;
    size_t count;
    size_t task_now;
    spinlock lock_spinlock;
};

static scheduler *current_scheduler;
static thread_local size_t number;

void new_scheduler();
void run_scheduler();

void submit(fiber_routine routine);
void spawn(fiber_routine routine);
void yield();
void terminate_scheduler();
void shutdown();
void notify_workers();