#include <structures/thin_heap.h>
#include <stdio.h>

const int MAX_RANK = 32;

void insert_to_thin_heap(thin_heap *heap, thin_node *temp)
{
    temp->rank = 0;
    temp->child = NULL;
    temp->left = NULL;
    temp->right = NULL;
    if (heap->first == NULL)
    {
        heap->first = temp;
        heap->last = temp;
    }
    else
    {
        if (heap->comparator(temp, heap->first) > 0)
        {
            temp->right = heap->first;
            heap->first = temp;
        }
        else
        {
            heap->last->right = temp;
            heap->last = temp;
        }
    }
    heap->size++;
}

static int is_thin(thin_node *node)
{
    if (node->rank == 0)
        return node->child == NULL;
    return node->rank != node->child->rank + 1;
}

thin_node *extract_min_thin_heap(thin_heap *heap)
{
    thin_node *temp;
    temp = heap->first;
    if (heap->first == NULL)
    {
        heap->last = NULL;
        heap->size = 0;
        return NULL;
    }
    heap->first = heap->first->right;
    thin_node *x = temp->child;
    thin_node *next = NULL;
    while (x)
    {
        if (is_thin(x))
        {
            x->rank -= 1;
        }
        next = x->right;
        x->left = NULL;
        x->right = NULL;
        insert_to_thin_heap(heap, x);
        x = next;
    }
    int max = -1;
    x = heap->first;
    thin_node *comb[MAX_RANK];
    for (int i = 0; i < MAX_RANK; i++)
    {
        comb[i] = NULL;
    }

    thin_node *stakan;
    while (x != NULL)
    {
        next = x->right;
        while (comb[x->rank] != NULL)
        {
            if (heap->comparator(comb[x->rank], x) > 0)
            {
                stakan = comb[x->rank];
                comb[x->rank] = x;
                x = stakan;
            }
            comb[x->rank]->right = x->child;
            if (x->child)
                x->child->left = comb[x->rank];

            comb[x->rank]->left = x;
            x->child = comb[x->rank];
            comb[x->rank] = NULL;
            x->rank++;
        }
        // x->right = NULL;
        comb[x->rank] = x;

        if (x->rank > max)
            max = x->rank;
        x = next;
    }
    heap->first = NULL;
    heap->last = NULL;
    heap->size = 0;
    printf("%d \n", max);
    for (int i = 0; i <= max; i++)
    {
        if (comb[i])
        {
            insert_to_thin_heap(heap, comb[i]);
        }
    }
    return temp;
}