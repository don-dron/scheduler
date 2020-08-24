#include <test_utils.h>

#define SCHEDS_COUNT 1
#define ROOT_ROUTINES TEST_LEVEL
#define ROOTINES_STEP TEST_LEVEL

static void joined_function()
{
    for (int i = 0; i < 1 << 15; i++)
    {
        __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
        sum++;

        __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
        sum++;
    }
}

static void internal_routine()
{
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    fiber *one = submit(joined_function, NULL);

    // FOR TEST
    usleep(5000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    sum++;
    __atomic_fetch_add(&interrupted, 2, __ATOMIC_SEQ_CST);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    fiber *two = submit(joined_function, NULL);

    // FOR TEST
    usleep(5000);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;
    join(two);

    __atomic_fetch_add(&atom, 1, __ATOMIC_SEQ_CST);
    sum++;

    join(one);
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