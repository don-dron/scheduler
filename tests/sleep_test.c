#include <test_utils.h>

static void func(void *args)
{
    sleep_for(1000000);
}

static void test()
{
    scheduler sched;
    new_default_scheduler(&sched);

    spawn(&sched, func, NULL);

    run_scheduler(&sched);
    terminate_scheduler(&sched);
}

int main()
{
    run_test(test);
}