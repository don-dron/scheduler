
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <context.h>
#include <coroutine.h>
#include <testTree.h>
#include <lf_stack.h>
#include <fiber.h>
#include <thin_heap.h>
#include <list.h>
#include <scheduler.h>

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

void kek()
{
}

void test3Foo()
{
    printf("%d\n", 88);
    for (int i = 0; i < 123; ++i)
    {
        coroutine first = create_coroutine_on_stack(kek);
        resume(&first);
        ++inner_step_count;
        suspend();

        free_coroutine_on_stack(&first);
    }
    printf("%d\n", 99);
}

void test3Bar()
{
    coroutine first = create_coroutine_on_stack(test3Foo);
    while (!first.complete)
    {
        resume(&first);
        suspend();
    }
    printf("%d\n", 55);
    // FreeCoroutine(first);
    printf("%d\n", 77);

    free_coroutine_on_stack(&first);
}

void test3()
{
    coroutine second = create_coroutine_on_stack(test3Bar);
    while (!second.complete)
    {
        resume(&second);
    }

    printf("%d\n", 66);
    assert(inner_step_count == kSteps);
    printf("%d == %d\n", inner_step_count, kSteps);

    free_coroutine_on_stack(&second);
}

void TreeWalk(TreeNode *node)
{
    if (node->left_)
    {
        TreeWalk(node->left_);
    }
    suspend();
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

    coroutine walker = create_coroutine_on_stack(walk);

    size_t node_count = 0;

    while (1)
    {
        resume(&walker);
        if (walker.complete)
        {
            break;
        }
        ++node_count;
    }

    assert(node_count == 7);

    free_coroutine_on_stack(&walker);
}

lf_stack *results;

struct stack_data_node
{
    stack_node lst_node;
    int data;
};
typedef struct stack_data_node stack_data_node;

void *worker(void *arg)
{
    int i = 1000;
    long long *int_data;
    while (i--)
    {
        int_data = (long long *)malloc(sizeof(long long));
        assert(int_data != NULL);
        *int_data = i;
        int data;
        for (int j = 0; j < 100; j++)
        {
            stack_data_node *stackNode = (stack_data_node *)malloc(sizeof(stack_data_node));
            stackNode->data = j;
            stackNode->lst_node.list_mutex = 0;

            push(results, stackNode);
        }
        for (int j = 0; j < 100; j++)
        {
            stack_data_node *node = pop(results);
            if (node != 0)
            {
                printf("%d\n", node->data);
                free(node);
            }
        }

        free(int_data);
    }

    return NULL;
}

void stack_test()
{
    results = create_stack();
    int nthreads = sysconf(_SC_NPROCESSORS_ONLN);
    int i;

    pthread_t threads[nthreads];
    printf("Using %d thread%s.\n", nthreads, nthreads == 1 ? "" : "s");
    for (i = 0; i < nthreads; i++)
        pthread_create(threads + i, NULL, worker, NULL);

    for (i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL);

    printf("Size = %d\n", results->size);

    free_stack(results);
}

list *lst;

struct data_node
{
    list_node lst_node;
    int data;
};

typedef struct data_node data_node;

void list_worker(void *arg)
{
    int i = 10000;
    long long *int_data;
    while (i--)
    {
        int_data = (long long *)malloc(sizeof(long long));
        assert(int_data != NULL);
        *int_data = i;
        int data;
        for (int j = 0; j < 100; j++)
        {
            data_node *nd = (data_node *)malloc(sizeof(data_node));
            nd->data = j;
            list_push_front(lst, nd);
        }
        for (int j = 0; j < 100; j++)
        {
            void *item = list_pop_front(lst);

            if (item != 0)
            {
                free(item);
            }
        }
        for (int j = 0; j < 100; j++)
        {
            data_node *nd = (data_node *)malloc(sizeof(data_node));
            nd->data = j;

            list_push_back(lst, nd);
        }
        for (int j = 0; j < 100; j++)
        {
            void *item = list_pop_back(lst);
            if (item != 0)
            {
                free(item);
            }
        }

        free(int_data);
    }
}

void list_test()
{
    lst = create_list();
    int nthreads = sysconf(_SC_NPROCESSORS_ONLN);
    int i;

    pthread_t threads[nthreads];
    for (i = 0; i < nthreads; i++)
        pthread_create(threads + i, NULL, list_worker, NULL);

    for (i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL);

    free_list(lst);
}

void list_test1()
{
    lst = create_list();

    data_node *nd = (data_node *)malloc(sizeof(data_node));
    nd->data = 10;
    list_push_front(lst, nd);

    void *item = list_pop_front(lst);

    if (item != 0)
    {
        free(item);
    }

    nd = (data_node *)malloc(sizeof(data_node));
    nd->data = 10;
    list_push_front(lst, nd);

    item = list_pop_front(lst);

    if (item != 0)
    {
        free(item);
    }

    free_list(lst);
}

int sum = 0;
int atom = 0;

void c()
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

void func()
{
    for (int i = 0; i < 100; i++)
    {
        submit(c);
        yield();
    }
}

int main()
{
    // printf("Start\n");
    // test1();
    // printf("Test 1 \n");
    // test2();
    // printf("Test 2 \n");
    // test3();
    // printf("Test 3 \n");
    // // treeTest();
    // printf("Test 4 \n");

    // stack_test();

    // list_test1();
    // list_test();

    new_scheduler();

    for (int i = 0; i < 100; i++)
    {
        spawn(func);
        spawn(func);
        spawn(func);
        spawn(func);
        spawn(func);
        spawn(func);
    }

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    run_scheduler();
    terminate_scheduler();

    gettimeofday(&stop, NULL);

    unsigned long delta_us = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);

    printf("Time microseconds %ld , milliseconds %ld\n",
           delta_us, delta_us / 1000);
    printf("atomic %d sum %d\n", atom, sum);
    return 0;
}