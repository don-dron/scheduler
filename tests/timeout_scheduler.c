#include <test_utils.h>

static void func(void *args)
{
    volatile int i = 10;
    while (i > 0)
    {
        sleep(100);
        --i;
    }
}

static void test()
{
    scheduler sched;
    new_scheduler(&sched, 8);

    for (size_t i = 0; i < 8; i++)
    {
        spawn(&sched, func, (void *)i);
    }

    run_scheduler(&sched);
    terminate_scheduler(&sched);
}

int main()
{
    run_test(test);
    return EXIT_SUCCESS;
}