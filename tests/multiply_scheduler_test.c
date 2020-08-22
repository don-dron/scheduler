#include <test_utils.h>

static void inner_to_inner(void *args)
{
    // Every functions plus 2
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    yield();

    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();
}

static void inner_func(void *args)
{
    // Every functions plus 2 and submit 2 functions
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    submit(inner_to_inner, NULL);
    sum++;

    yield();

    submit(inner_to_inner, NULL);

    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();

    // All sum this function is 2 + 2 * 2
}

static void func()
{
    // Every function submits 300 functions
    for (int i = 0; i < 20; i++)
    {
        submit(inner_func, NULL);
        yield();
        submit(inner_func, NULL);
        yield();
        submit(inner_func, NULL);
    }

    // All sum this function is 300 * (2 + 2 * 2)
}

static void test()
{
    scheduler sched, sched1;

    new_default_scheduler(&sched);
    new_default_scheduler(&sched1);
    for (int i = 0; i < 50; i++)
    {
        // 12 * functions
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
    }

    // All sum is 300 * 1200 * (2 + 2 * 2) = 360000 * 6 = 2 160 000 - right answer

    run_scheduler(&sched);
    run_scheduler(&sched1);

    shutdown(&sched);
    shutdown(&sched1);

    for (int i = 0; i < 50; i++)
    {
        // 12 * functions
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
    }

    run_scheduler(&sched);
    run_scheduler(&sched1);

    shutdown(&sched);
    shutdown(&sched1);

    terminate_scheduler(&sched);
    terminate_scheduler(&sched1);

    // assert(atom == 4320000);
}

int main()
{
    run_test(test);
    return EXIT_SUCCESS;
}