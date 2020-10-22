#include <test_utils.h>

#define ROOT_ROUTINES TEST_LEVEL

static void func(void *args)
{
    sleep_for(2000);
}

static void test()
{
    scheduler sched;
    new_scheduler(&sched, (unsigned int)scheds_threads);

    for (int i = 0; i < ROOT_ROUTINES; i++)
    {
        spawn(&sched, func, NULL);
    }

    run_scheduler(&sched);
    terminate_scheduler(&sched);
}

int main()
{
    run_test(test);
}