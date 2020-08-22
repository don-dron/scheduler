#include <test_utils.h>

#define ROOT_ROUTINES 1
#define ROOTINES_STEP 1

static void inner_to_inner(void *args)
{
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
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    submit(inner_to_inner, NULL);
    sum++;

    yield();

    submit(inner_to_inner, NULL);

    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    yield();
}

static void func()
{
    for (int i = 0; i < ROOTINES_STEP; i++)
    {
        submit(inner_func, NULL);
        yield();
        submit(inner_func, NULL);
        yield();
        submit(inner_func, NULL);
    }
}

static void test()
{
    scheduler sched, sched1;

    new_scheduler(&sched,(unsigned int)scheds_threads);
    new_scheduler(&sched1,(unsigned int)scheds_threads);
    for (int i = 0; i < ROOT_ROUTINES; i++)
    {
        spawn(&sched, func, NULL);
        spawn(&sched1, func, NULL);
    }

    run_scheduler(&sched);
    run_scheduler(&sched1);

    shutdown(&sched);
    shutdown(&sched1);

    for (int i = 0; i < ROOT_ROUTINES; i++)
    {
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