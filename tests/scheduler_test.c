#include <test_utils.h>

static void inner_func(void *args)
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

static void func(void* args)
{
    for (int i = 0; i < 100; i++)
    {
        submit(inner_func, NULL);
        yield();
    }
}

static void test()
{
    scheduler sched;
    new_default_scheduler(&sched);

    for (int i = 0; i < 100; i++)
    {
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched, func, NULL);
    }

    run_scheduler(&sched);
    terminate_scheduler(&sched);

    // assert(atom == 120000);
}

int main()
{
    run_test(test);
    return EXIT_SUCCESS;
}