
#define __USE_MISC 1
#include "context/context.h"
#include "coroutine/coroutine.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "testTree.h"
#include "lockfree/stack/stack.h"

int step = 0;

void test1Foo()
{
    assert(step == 0);
    step = 1;
    Suspend();
    assert(step == 2);
    step = 3;
}

void test1Bar()
{
    assert(step == 1);
    step = 2;
    Suspend();
    assert(step == 3);
    step = 4;
}

void test1()
{
    Coroutine first = NewCoroutineOnStack(test1Foo);
    Coroutine second = NewCoroutineOnStack(test1Bar);

    Resume(&first);
    Resume(&second);

    Resume(&first);
    Resume(&second);

    FreeCoroutineOnStack(&first);
    FreeCoroutineOnStack(&second);
}

void test2Foo()
{
    for (int i = 0; i < 100; i++)
    {
        Suspend();
    }
}

void test2Bar()
{
    for (int i = 0; i < 100; i++)
    {
        Suspend();
    }
}

void test2()
{
    Coroutine first = NewCoroutineOnStack(test2Foo);
    Coroutine second = NewCoroutineOnStack(test2Bar);

    while (!(first.complete && second.complete))
    {
        Resume(&first);
        Resume(&second);
    }

    FreeCoroutineOnStack(&first);
    FreeCoroutineOnStack(&second);
}

const int kSteps = 123;

int inner_step_count = 0;

void kek()
{
}

void test3Foo()
{
    printf("%d\n", 88);
    for (int i = 0; i <123; ++i)
    {
        Coroutine first = NewCoroutineOnStack(kek);
        Resume(&first);
        ++inner_step_count;
        Suspend();

        FreeCoroutineOnStack(&first);
    }
    printf("%d\n", 99);
}

void test3Bar()
{
    Coroutine first = NewCoroutineOnStack(test3Foo);
    while (!first.complete)
    {
        Resume(&first);
        Suspend();
    }
    printf("%d\n", 55);
    // FreeCoroutine(first);
    printf("%d\n", 77);

    FreeCoroutineOnStack(&first);
}

void test3()
{
    Coroutine second = NewCoroutineOnStack(test3Bar);
    while (!second.complete)
    {
        Resume(&second);
    }

    printf("%d\n", 66);
    assert(inner_step_count == kSteps);
    printf("%d == %d\n", inner_step_count, kSteps);
    
    FreeCoroutineOnStack(&second);
}

void TreeWalk(TreeNode *node)
{
    if (node->left_)
    {
        TreeWalk(node->left_);
    }
    Suspend();
    if (node->right_)
    {
        TreeWalk(node->right_);
    }
}
TreeNode *root;

void walk()
{
    TreeWalk(root);
}

void treeTest()
{
    root = Create(
        CreateLeaf(),
        Create(
            Create(CreateLeaf(), CreateLeaf()),
            CreateLeaf()));

    Coroutine walker = NewCoroutineOnStack(walk);

    size_t node_count = 0;

    while (1)
    {
        Resume(&walker);
        if (walker.complete)
        {
            break;
        }
        ++node_count;
    }

    assert(node_count == 7);

    FreeCoroutineOnStack(&walker);
}

void stackTest()
{
    LockFreeStack *stack = NewStack();

    Push(stack, 1);
    Push(stack, 2);

    int item = 0;

    Pop(stack, &item);
    assert(2 == item);

    Pop(stack, &item);
    assert(1 == item);

    FreeStack(stack);
}

int main()
{
    printf("Start\n");
    test1();
    printf("Test 1 \n");
    test2();
    printf("Test 2 \n");
    test3();
    printf("Test 3 \n");
    // treeTest();
    printf("Test 4 \n");
    
    stackTest();
    return 0;
}