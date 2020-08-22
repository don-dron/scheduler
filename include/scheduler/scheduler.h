#pragma once

#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <threads.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h> 
#include <errno.h>

#include <scheduler/fiber.h>
#include <structures/list.h>
#include <locks/atomics.h>

typedef struct scheduler
{
    struct sigaction sigact;
    size_t threads;
    list **queues;
    pthread_t* threads_pool;
    pthread_t main_thread;
    volatile int threads_running;
    volatile int terminate;
    volatile size_t count;
    volatile size_t end_count;
    spinlock lock_spinlock;
} scheduler;

extern thread_local unsigned long pid;
extern thread_local scheduler *current_scheduler;

int new_default_scheduler(scheduler *sched);
int new_scheduler(scheduler *sched, unsigned int using_threads);

void run_scheduler(scheduler *sched);

fiber *submit(fiber_routine routine, void *args);
fiber *spawn(scheduler *sched, fiber_routine routine, void *args);
void join(fiber *fib);
void yield(void);
int terminate_scheduler(scheduler *sched);
void shutdown(scheduler *sched);
void sleep_for(unsigned long duration);