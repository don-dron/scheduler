#include <test_utils.h>

#define ROOT_ROUTINES TEST_LEVEL

static void func(void *args)
{
    volatile int i = 5;
    while (i > 0)
    {
        sleep_for(3000);
        --i;
    }
}

static void test()
{
    scheduler sched;
    new_scheduler(&sched, (unsigned int)scheds_threads);

    for (size_t i = 0; i < ROOT_ROUTINES; i++)
    {
        spawn(&sched, func, (void *)i);
    }

    run_scheduler(&sched);
    shutdown(&sched);
    terminate_scheduler(&sched);
}

int main()
{
    run_test(test);
    return EXIT_SUCCESS;
}