
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/scheduler.h>

void func()
{
    sleep_for(1);
}

int main()
{
    scheduler *sched = new_default_scheduler();

    for (int i = 0; i < 1; i++)
    {
        spawn(sched, func);
        spawn(sched, func);
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

    printf("PASSED\n");
    return EXIT_SUCCESS;
}