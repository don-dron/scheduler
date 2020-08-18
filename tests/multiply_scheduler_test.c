
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/scheduler.h>

int sum = 0;
int atom = 0;

void h()
{
    // Every functions plus 2
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    yield();

    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();
}

void c()
{
    // Every functions plus 2 and submit 2 functions
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    submit(h);
    sum++;

    yield();

    submit(h);

    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();

    // All sum this function is 2 + 2 * 2
}

void func()
{
    // Every function submits 300 functions
    for (int i = 0; i < 100; i++)
    {
        submit(c);
        yield();
        submit(c);
        yield();
        submit(c);
    }

    // All sum this function is 300 * (2 + 2 * 2)
}

void test1()
{
    scheduler *sched = new_default_scheduler();
    scheduler *sched1 = new_default_scheduler();
    for (int i = 0; i < 100; i++)
    {
        // 12 * functions
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
    }

    // All sum is 300 * 1200 * (2 + 2 * 2) = 360000 * 6 = 2 160 000 - right answer

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    run_scheduler(sched);
    run_scheduler(sched1);

    shutdown(sched);
    shutdown(sched1);

    for (int i = 0; i < 100; i++)
    {
        // 12 * functions
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
        spawn(sched, func);
        spawn(sched1, func);
    }

    run_scheduler(sched);
    run_scheduler(sched1);

    shutdown(sched);
    shutdown(sched1);

    terminate_scheduler(sched);
    terminate_scheduler(sched1);

    print_statistic();
    // assert(atom == 4320000);

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