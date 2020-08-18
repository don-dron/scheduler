
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include <scheduler/scheduler.h>

int sum = 0;
int atom = 0;

void c()
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

void func()
{
    for (int i = 0; i < 100; i++)
    {
        submit(c);
        yield();
    }
}

void test1()
{
    scheduler *sched = new_default_scheduler();

    for (int i = 0; i < 100; i++)
    {
        spawn(sched, func);
        spawn(sched, func);
        spawn(sched, func);
        spawn(sched, func);
        spawn(sched, func);
        spawn(sched, func);
    }

    run_scheduler(sched);
    terminate_scheduler(sched);

    print_statistic();
    // assert(atom == 120000);

    printf("%ld %ld\n", atom, sum);
}

void run_test(void (*test)())
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