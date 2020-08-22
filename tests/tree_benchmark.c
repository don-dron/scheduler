#include <test_utils.h>

#define SCHEDS_COUNT 1
#define ROOT_ROUTINES 1
#define ROOTINES_STEP 1

static void internal_routine()
{
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    // Work emulation
    sleep_for(2 * 1000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    usleep(50000);

    for (int i = 0; i < 1 << 10; i++)
    {
        sum += i;
        __atomic_fetch_add(&atom, i, __ATOMIC_SEQ_CST);

            sum -= i;
        __atomic_fetch_sub(&atom, i, __ATOMIC_SEQ_CST);
    }

    usleep(50000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    sum++;
    __atomic_fetch_add(&interrupted, 2, __ATOMIC_SEQ_CST);

    // Work emulation
    sleep_for(2 * 1000);

    // Work emulation
    sleep_for(2 * 1000);
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    // Work emulation
    sleep_for(2 * 1000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    yield();
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
}

static void root_routine()
{
    fiber **steps = (fiber **)malloc(ROOTINES_STEP * sizeof(fiber *));

    for (int i = 0; i < ROOTINES_STEP; i++)
    {
        steps[i] = submit(internal_routine, NULL);
    }

    for (int i = 0; i < ROOTINES_STEP; i++)
    {
        join(steps[i]);
    }
}

static void tree()
{
    scheduler **scheds = (scheduler **)malloc(SCHEDS_COUNT * sizeof(scheduler *));

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        scheds[i] = (scheduler *)malloc(sizeof(scheduler));
        new_scheduler(scheds[i], (unsigned int)scheds_threads);
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        for (int j = 0; j < ROOT_ROUTINES; j++)
        {
            spawn(scheds[i], root_routine, NULL);
        }
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        run_scheduler(scheds[i]);
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        for (int j = 0; j < ROOT_ROUTINES; j++)
        {
            spawn(scheds[i], root_routine, NULL);
        }
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        shutdown(scheds[i]);
    }

    for (int i = 0; i < SCHEDS_COUNT; i++)
    {
        terminate_scheduler(scheds[i]);
    }
}

int main()
{
    run_test(tree);
    return EXIT_SUCCESS;
}