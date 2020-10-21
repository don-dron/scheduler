
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <structures/fibonacci_heap.h>

#define COUNT 100

static int cmp(void *lhs, void *rhs)
{
    if (((unsigned long)lhs) < ((unsigned long)rhs))
    {
        return -1;
    }
    else if (((unsigned long)lhs) > ((unsigned long)rhs))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void *key_min(void)
{
    struct fibonacci_heap_node *node = fibonacci_heap_node_alloc();
    node->key = NULL;
    return node;
}

static void key_pr(void *key)
{
}

static void test()
{
    struct fibonacci_heap *results = fibonacci_heap_alloc(cmp, key_min, key_pr);

    int *flags = (int *)malloc(sizeof(int) * COUNT);
    memset(flags, 0, sizeof(int) * COUNT);

    for (int i = 0; i < COUNT; i++)
    {
        struct fibonacci_heap_node *node = fibonacci_heap_node_alloc();
        node->key = (void *)(u_int64_t)(rand() % COUNT);
        flags[(size_t)node->key] = 1;
        fibonacci_heap_insert_node(results, node);
    }

    struct fibonacci_heap_node *prev = (struct fibonacci_heap_node *)fibonacci_heap_extract_min_node(results);

    while (true)
    {
        flags[(size_t)prev->key] = 0;
        struct fibonacci_heap_node *current = (struct fibonacci_heap_node *)fibonacci_heap_extract_min_node(results);

        if (current == NULL)
        {
            break;
        }
        assert(current->key >= prev->key);
        printf("%ld %ld\n", (unsigned long)current->key, (unsigned long)prev->key);
        prev = current;
    }

    int sum = 0;
    for (int i = 0; i < COUNT; i++)
    {
        sum += flags[i];
    }

    assert(sum == 0);
    free(results);
}

int main()
{
    test();
    printf("PASSED\n");
    return EXIT_SUCCESS;
}