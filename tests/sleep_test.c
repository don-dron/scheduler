#include <test_utils.h>

static void func(void *args)
{
    sleep_for(10000);
}

static void test()
{
    scheduler sched;
    new_scheduler(&sched, (unsigned int)scheds_threads);

    spawn(&sched, func, NULL);

    run_scheduler(&sched);
    terminate_scheduler(&sched);
}

int main()
{
    run_test(test);
}