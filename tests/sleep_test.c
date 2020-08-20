
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/scheduler.h>

static void func(void* args)
{
    sleep_for(1);
}

int main()
{
    scheduler sched;
    new_default_scheduler(&sched);

    for (int i = 0; i < 1; i++)
    {
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
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

    printf("PASSED\n");
    return EXIT_SUCCESS;
}