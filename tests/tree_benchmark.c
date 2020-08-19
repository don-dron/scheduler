
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/scheduler.h>

#define SCHEDS_COUNT 4
#define SCHEDSS_THREADS 4
#define ROOT_ROUTINES 4
#define ROOTINES_STEP 4

static int sum = 0;
static int atom = 0;

static void internal_routine()
{
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    // Work emulation
    sleep_for(2 * 1000 * 1000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    // Work emulation
    sleep_for(2 * 1000 * 1000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    // Work emulation
    sleep_for(2 * 1000 * 1000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
}

static void root_routine()
{
    fiber **steps = (fiber **)malloc(ROOTINES_STEP * sizeof(fiber *));

    for (int i = 0; i < ROOTINES_STEP; i++)
    {
        steps[i] = submit(internal_routine, NULL);
    }

    for (int i = 0; i < ROOTINES_STEP; i++)
    {
        join(steps[i]);
    }
}

static void tree()
{
    scheduler **scheds = (scheduler **)malloc(SCHEDS_COUNT * sizeof(scheduler *));

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        scheds[i] = new_scheduler(SCHEDSS_THREADS);
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        for (int j = 0; j < ROOT_ROUTINES; j++)
        {
            spawn(scheds[i], root_routine, NULL);
        }
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        run_scheduler(scheds[i]);
    }

    usleep(1);

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        for (int j = 0; j < ROOT_ROUTINES; j++)
        {
            spawn(scheds[i], root_routine, NULL);
        }
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        shutdown(scheds[i]);
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        terminate_scheduler(scheds[i]);
    }
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
    printf("%d %d\n", atom, sum);
}

int main()
{
    run_test(tree);
    printf("PASSED\n");
    return EXIT_SUCCESS;
}