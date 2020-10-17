#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <scheduler/context.h>
#include <structures/list.h>

#ifndef TEST_LEVEL
#define TEST_LEVEL 5
#endif

#if RB_TREE
#include <scheduler/rb_tree_scheduler.h>
#elif LOCAL_QUEUES
#include <scheduler/local_queues_scheduler.h>
#elif THIN_HEAP
#include <scheduler/thin_heap_scheduler.h>
#elif ELASTIC_TREE
#include <scheduler/elastic_tree_scheduler.h>
#elif SPLAY_TREE
#include <scheduler/splay_tree_scheduler.h>
#elif LOCAL_QUEUES_WITH_STEAL
#include <scheduler/local_queues_with_steal_scheduler.h>
#else
#error "Scheduler not defined"
#endif

typedef struct free_node {
    list_node node;
    fiber *fib;
}
free_node;

int sum = 0;
int atom = 0;
unsigned long scheds_threads = 1;
unsigned long interrupted = 0;

void run_test(void (*test)());

void run_test(void (*test)())
{
#if FIBER_STAT
    scheds_threads = 16;
    create_history();

    test();

    print_history();

    free_history();
#elif THREAD_STAT
    scheds_threads = 16;
    create_history();

    test();

    print_history();

    free_history();
#else
    printf("thread time interrupt interrupt_fail switch \n");
    while (scheds_threads <= 64UL)
    {
        struct timespec mt1, mt2;
        unsigned long int delta;
        clock_gettime(CLOCK_REALTIME, &mt1);

        test();

        clock_gettime(CLOCK_REALTIME, &mt2);
        delta = 1000UL * 1000UL * 1000UL * (unsigned long)(mt2.tv_sec - mt1.tv_sec) + (unsigned long)(mt2.tv_nsec - mt1.tv_nsec);

        statistic stat = get_statistic();

        printf("%ld %ld %ld %ld %ld \n", scheds_threads, delta / 1000, stat.interrupt_count, stat.interrupt_failed_count, stat.switch_count);

        scheds_threads+=3;
    }
#endif
}
