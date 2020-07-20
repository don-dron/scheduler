
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <structures/lf_stack.h>

lf_stack *results;

struct stack_data_node
{
    lf_stack_node lst_node;
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

            push_lf_stack(results, stackNode);
        }
        for (int j = 0; j < 100; j++)
        {
            stack_data_node *node = pop_lf_stack(results);
            if (node != 0)
            {
                free(node);
            }
        }

        free(int_data);
    }

    return NULL;
}

void stack_test()
{
    results = create_lf_stack();
    int nthreads = sysconf(_SC_NPROCESSORS_ONLN);
    int i;

    pthread_t threads[nthreads];
    printf("Using %d thread%s.\n", nthreads, nthreads == 1 ? "" : "s");
    for (i = 0; i < nthreads; i++)
        pthread_create(threads + i, NULL, worker, NULL);

    for (i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL);

    printf("Size = %d\n", results->size);
    assert(results->size == 0);

    free_lf_stack(results);
}

int main()
{
    stack_test();
    return EXIT_SUCCESS;
}