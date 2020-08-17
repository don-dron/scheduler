
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

int main()
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

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    run_scheduler(sched);
    terminate_scheduler(sched);

    print_statistic();
    assert(atom == 120000);

    printf("%ld %ld\n", atom, sum);

    return EXIT_SUCCESS;
}