
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <scheduler/coroutine.h>

int step = 0;

void test1Foo()
{
    assert(step == 0);
    step = 1;
    suspend();
    assert(step == 2);
    step = 3;
}

void test1Bar()
{
    assert(step == 1);
    step = 2;
    suspend();
    assert(step == 3);
    step = 4;
}

void test1()
{
    coroutine first = create_coroutine_on_stack(test1Foo);
    coroutine second = create_coroutine_on_stack(test1Bar);

    resume(&first);
    resume(&second);

    resume(&first);
    resume(&second);

    free_coroutine_on_stack(&first);
    free_coroutine_on_stack(&second);
}

void test2Foo()
{
    for (int i = 0; i < 100; i++)
    {
        suspend();
    }
}

void test2Bar()
{
    for (int i = 0; i < 100; i++)
    {
        suspend();
    }
}

void test2()
{
    coroutine first = create_coroutine_on_stack(test2Foo);
    coroutine second = create_coroutine_on_stack(test2Bar);

    while (!(first.complete && second.complete))
    {
        resume(&first);
        resume(&second);
    }

    free_coroutine_on_stack(&first);
    free_coroutine_on_stack(&second);
}

const int kSteps = 123;

int inner_step_count = 0;

void nop()
{
    // nop
}

void test3Foo()
{
    for (int i = 0; i < 123; ++i)
    {
        coroutine first = create_coroutine_on_stack(nop);
        resume(&first);
        ++inner_step_count;
        suspend();

        free_coroutine_on_stack(&first);
    }
}

void test3Bar()
{
    coroutine first = create_coroutine_on_stack(test3Foo);
    while (!first.complete)
    {
        resume(&first);
        suspend();
    }
    free_coroutine_on_stack(&first);
}

void test3()
{
    coroutine second = create_coroutine_on_stack(test3Bar);
    while (!second.complete)
    {
        resume(&second);
    }

    assert(inner_step_count == kSteps);
    free_coroutine_on_stack(&second);
}

int main()
{
    test1();
    test2();
    test3();
    return EXIT_SUCCESS;
}