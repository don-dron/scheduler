#include <test_utils.h>

#define ROOT_ROUTINES TEST_LEVEL

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
    for (int i = 0; i < ROOT_ROUTINES; i++)
    {
        submit(inner_func, NULL);
        yield();
    }
}

static void test()
{
    scheduler sched;
    new_scheduler(&sched,(unsigned int)scheds_threads);

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
    return EXIT_SUCCESS;
}