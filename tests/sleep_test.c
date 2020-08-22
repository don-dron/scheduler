
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/scheduler.h>

static void func(void *args)
{
    sleep_for(1000000);
}

static void test()
{
    scheduler sched;
    new_default_scheduler(&sched);

    spawn(&sched, func, NULL);

    run_scheduler(&sched);
    terminate_scheduler(&sched);
}

static void run_test(void (*test)())
{
    struct timespec mt1, mt2;
    long int delta;
    clock_gettime(CLOCK_REALTIME, &mt1);

    test();

    clock_gettime(CLOCK_REALTIME, &mt2);
    delta = 1000 * 1000 * 1000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);

    print_statistic();
    printf("Time: microseconds %ld\n", delta / 1000);
    printf("Time: milliseconds %ld\n", delta / 1000 / 1000);
    printf("Time: seconds %ld\n", delta / 1000 / 1000 / 1000);
}

int main()
{
    run_test(test);
}