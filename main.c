#include "context/context.h"
#include "coroutine/coroutine.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "testTree.h"

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
    Coroutine *first = NewCoroutine(test1Foo);
    Coroutine *second = NewCoroutine(test1Bar);

    Resume(first);
    Resume(second);

    Resume(first);
    Resume(second);

    FreeCoroutine(first);
    FreeCoroutine(second);
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
    Coroutine *first = NewCoroutine(test2Foo);
    Coroutine *second = NewCoroutine(test2Bar);

    while (!(first->complete && second->complete))
    {
        Resume(first);
        Resume(second);
    }

    FreeCoroutine(first);
    FreeCoroutine(second);
}

const size_t kSteps = 123;

size_t inner_step_count = 0;

void test3Foo()
{
    for (size_t i = 0; i < kSteps; ++i)
    {
        ++inner_step_count;
        Suspend();
    }
}

void test3Bar()
{
    Coroutine *first = NewCoroutine(test3Foo);
    while (!first->complete)
    {
        Resume(first);
        Suspend();
    }
}

void test3()
{
    Coroutine *second = NewCoroutine(test3Bar);
    while (!second->complete)
    {
        Resume(second);
    }

    assert(inner_step_count == kSteps);
    printf("%d == %d\n", inner_step_count, kSteps);
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

    Coroutine *walker = NewCoroutine(walk);

    size_t node_count = 0;

    while (1)
    {
        Resume(walker);
        if (walker->complete)
        {
            break;
        }
        ++node_count;
    }

    assert(node_count == 7);
}
int main()
{
    printf("Start\n");
    test1();
    printf("Test 1 \n");
    test2();
    printf("Test 2 \n");
    // test3();
    // printf("Test 3 \n");
    // treeTest();
    // printf("Test 4 \n");
    return 0;
}