
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

#include <structures/thin_heap.h>

#define COUNT 100

typedef struct fiber_node
{
    struct thin_heap_node lst_node;
    int value;
} fiber_node;

static int cmp(const void *lhs, const void *rhs)
{
    fiber_node *first = (fiber_node *)lhs;
    fiber_node *second = (fiber_node *)rhs;

    return first->value < second->value;
}

static void swp(struct thin_heap_node *first, struct thin_heap_node *second)
{
    fiber_node *f = (fiber_node *)first;
    fiber_node *s = (fiber_node *)second;
    int temp;

    temp = f->value;
    f->value = s->value;
    s->value = temp;
}

static void test()
{
    struct thin_heap *results = (struct thin_heap *)malloc(sizeof(struct thin_heap));
    heap_init(results, (heap_prio_t)cmp, (swap_f)swp);

    int *flags = (int *)malloc(sizeof(int) * COUNT);
    memset(flags, 0, sizeof(int) * COUNT);

    for (int i = 0; i < COUNT; i++)
    {
        fiber_node *node = (fiber_node *)malloc(sizeof(fiber_node));
        node->value = rand() % COUNT;
        flags[node->value] = 1;
        heap_insert(results, &node->lst_node);
    }

    fiber_node *prev = (fiber_node *)heap_take(results);

    while (true)
    {
        flags[prev->value] = 0;
        fiber_node *current = (fiber_node *)heap_take(results);

        if (current == NULL)
        {
            break;
        }
        assert(current->value >= prev->value);
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