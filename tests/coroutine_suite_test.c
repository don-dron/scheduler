
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/coroutine.h>

int step = 0;

static void test1Foo(void *args)
{
    assert(step == 0);
    step = 1;
    suspend();
    assert(step == 2);
    step = 3;
}

static void test1Bar(void *args)
{
    assert(step == 1);
    step = 2;
    suspend();
    assert(step == 3);
    step = 4;
}

static void test1()
{
    coroutine first = create_coroutine(test1Foo, NULL);
    coroutine second = create_coroutine(test1Bar, NULL);

    resume(&first);
    resume(&second);

    resume(&first);
    resume(&second);

    free_coroutine(&first);
    free_coroutine(&second);
}

static void test2Foo(void *args)
{
    for (int i = 0; i < 100; i++)
    {
        suspend();
    }
}

static void test2Bar(void *args)
{
    for (int i = 0; i < 100; i++)
    {
        suspend();
    }
}

static void test2()
{
    coroutine first = create_coroutine(test2Foo, NULL);
    coroutine second = create_coroutine(test2Bar, NULL);

    while (!(first.complete && second.complete))
    {
        resume(&first);
        resume(&second);
    }

    free_coroutine(&first);
    free_coroutine(&second);
}

const int kSteps = 123;

int inner_step_count = 0;

static void nop(void *args)
{
    // nop
}

static void test3Foo(void *args)
{
    for (int i = 0; i < 123; ++i)
    {
        coroutine first = create_coroutine(nop, NULL);
        resume(&first);
        ++inner_step_count;
        suspend();

        free_coroutine(&first);
    }
}

static void test3Bar(void *args)
{
    coroutine first = create_coroutine(test3Foo, NULL);
    while (!first.complete)
    {
        resume(&first);
        suspend();
    }
    free_coroutine(&first);
}

static void test3()
{
    coroutine second = create_coroutine(test3Bar, NULL);
    while (!second.complete)
    {
        resume(&second);
    }

    assert(inner_step_count == kSteps);
    free_coroutine(&second);
}

int main()
{
    test1();
    test2();
    test3();
    print_statistic();
    printf("PASSED\n");
    return EXIT_SUCCESS;
}