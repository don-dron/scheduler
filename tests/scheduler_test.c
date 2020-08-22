
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include <scheduler/local_queues_with_steal_scheduler.h>

static int sum = 0;
static int atom = 0;

static void inner_func(void *args)
{
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();
    sum++;
    yield();
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();
}

static void func(void* args)
{
    for (int i = 0; i < 100; i++)
    {
        submit(inner_func, NULL);
        yield();
    }
}

static void test1()
{
    scheduler sched;
    new_default_scheduler(&sched);

    for (int i = 0; i < 100; i++)
    {
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
    }

    run_scheduler(&sched);
    terminate_scheduler(&sched);

    print_statistic();
    assert(atom == 120000);

    printf("%d %d\n", atom, sum);
}

static void run_test(void (*test)())
{
    struct timespec mt1, mt2;
    long int delta;
    clock_gettime(CLOCK_REALTIME, &mt1);

    test();

    clock_gettime(CLOCK_REALTIME, &mt2);
    delta = 1000*1000*1000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);

    printf("Time: microseconds %ld\n", delta/1000);
}

int main()
{
    run_test(test1);

    printf("PASSED\n");
    return EXIT_SUCCESS;
}