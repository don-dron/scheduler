#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#if LOCAL_QUEUES_WITH_STEAL
#include <scheduler/local_queues_with_steal_scheduler.h>
#else
#error "Scheduler not defined"
#endif

#ifndef SCHEDSS_THREADS
#define SCHEDSS_THREADS 4
#endif

#define SCHEDS_COUNT 1
#define ROOT_ROUTINES 1
#define ROOTINES_STEP 16

int sum = 0;
int atom = 0;
unsigned long interrupted = 0;

void run_test(void (*test)());

void run_test(void (*test)())
{
    printf("Run test ... ");
    struct timespec mt1, mt2;
    long int delta;
    clock_gettime(CLOCK_REALTIME, &mt1);

    test();

    clock_gettime(CLOCK_REALTIME, &mt2);
    delta = 1000 * 1000 * 1000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);

    print_statistic();
    // printf("Time: microseconds %ld\n", delta / 1000);
    printf("Time: milliseconds %ld ", delta / 1000 / 1000);
    // printf("Time: seconds %ld\n", delta / 1000 / 1000 / 1000);
    // printf("%d %d\n", atom, sum);
    // printf("Interrupted %ld\n", interrupted);

    printf("PASSED\n");
}
