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

int sum = 0;
int atom = 0;
unsigned long scheds_threads = 1;
unsigned long interrupted = 0;

void run_test(void (*test)());

void run_test(void (*test)())
{
    printf("thread time\n");
    while (scheds_threads <= 64UL)
    {
        printf("%ld ", scheds_threads);
        struct timespec mt1, mt2;
        unsigned long int delta;
        clock_gettime(CLOCK_REALTIME, &mt1);

        test();

        clock_gettime(CLOCK_REALTIME, &mt2);
        delta = 1000UL * 1000UL * 1000UL * (unsigned long)(mt2.tv_sec - mt1.tv_sec) + (unsigned long)(mt2.tv_nsec - mt1.tv_nsec);

        printf("%ld\n", delta / 1000);

        if (scheds_threads == 1)
        {
            scheds_threads++;
        }
        else
        {
            scheds_threads += 2;
        }
    }
}
