
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

static int cmp(struct fib_heap_data* lhs, struct fib_heap_data* rhs)
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

static void test()
{
    struct fib_heap *results = fibheap_init(cmp);

    int *flags = (int *)malloc(sizeof(int) * COUNT);
    memset(flags, 0, sizeof(int) * COUNT);

    for (int i = 0; i < COUNT; i++)
    {
        void* data = (void *)((__uint64_t)(rand() % COUNT) + 1);
        flags[(size_t)data - 1] = 1;
        fibheap_insert(results, data);
    }

    __uint64_t prev = (__uint64_t)fibheap_extract(results);

    while (true)
    {
        flags[(size_t)prev - 1] = 0;
        __uint64_t current = (__uint64_t)fibheap_extract(results);

        if (current == 0)
        {
            break;
        }
        assert(current >= prev);
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