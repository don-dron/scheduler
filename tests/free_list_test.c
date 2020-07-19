
#define __USE_MISC 1

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include <structures/list.h>

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

void concurrent_test()
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

void simple_test()
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

int main()
{
    simple_test();
    concurrent_test();
    return EXIT_SUCCESS;
}