#pragma once

#include <time.h>
// #include <sys/time.h>
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
#include <locks/atomics.h>

typedef struct scheduler_manager scheduler_manager;

/** 
 *  Scheduler structure.
 */
typedef struct scheduler
{
    /** Signals preferences **/
    struct sigaction sigact;

    /** Map - thread number to pointer to thread - array **/
    fiber ***current_fibers;

    /** Count of handlers-threads **/
    size_t threads;

    /** Thread for sends signals **/
    pthread_t main_thread;
    /** Threads for handles fibers **/
    pthread_t *threads_pool;

    /** Flag sets 0 , after call run_scheduler sets to 1, after call shutdown sets to 0 **/
    volatile int threads_running;
    /** Flag sets if user wants to terminate scheduler **/
    volatile int terminate;

    /** Count fibers which not terminated **/
    volatile size_t count;
    /** Count fibers which terminated **/
    volatile size_t end_count;

    scheduler_manager* manager;
} scheduler;

/** Handler thread id **/
extern thread_local unsigned long thread_id;
/** External scheduler **/
extern thread_local scheduler *current_scheduler;

/** 
 * Create scheduler manager 
 */
int create_scheduler_manager(scheduler *sched);

/** 
 *  Returns fiber from pool. Function for override.
 */
fiber *get_from_pool(void);

/** 
 *  Puts fiber to pool. Function for override.
 */
void return_to_pull(scheduler *sched, fiber *fib);

/** 
 * Free scheduler manager 
 */
int free_scheduler_manager(scheduler *sched);

/** 
 *  Delegate call to new_scheduler function with param
 *  using_threads = core counts - 1.
 */
int new_default_scheduler(scheduler *sched);

/** 
 *  Create scheduler with using_threads handlers-threads.
 */
int new_scheduler(scheduler *sched, unsigned int using_threads);

/** 
 *  Run scheduler's fiber handlers.
 */
void run_scheduler(scheduler *sched);

/** 
 *  Function create fiber for scheduler. 
 */
fiber *spawn(scheduler *sched, fiber_routine routine, void *args);

/** 
 *  Function should be call in fiber stack frame. It's call create new fiber
 *  for routine and arguments.
 */
fiber *submit(fiber_routine routine, void *args);

/** 
 *  Delegate proccesor time to other fiber.
 */
void yield(void);

/** 
 *  Sleep fiber for microseconds.
 */
void sleep_for(unsigned long duration);

/** 
 *  Block current thread while all fibers not terminated.
 */
void shutdown(scheduler *sched);

/** 
 *  Block fiber while input fiber not terminated.
 */
void join(fiber *fib);

/** 
 *  Terminate scheduler. Shutdown all fibers and after free memory.
 */
int terminate_scheduler(scheduler *sched);