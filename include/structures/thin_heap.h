#pragma once

#include <stdlib.h>

struct thin_node;

typedef struct thin_node
{
    struct thin_node *child;
    struct thin_node *right;
    struct thin_node *left;
    int rank;
} thin_node;

typedef int (*THIN_HEAP_COMPARATOR)(thin_node *, thin_node *);

typedef void (*DELETE_DECREASE_KEY_FUNCTION)(thin_node *);

typedef void (*DECREASE_KEY_FUNCTION)(thin_node *);

typedef struct thin_heap
{
    thin_node *first;
    thin_node *last;
    THIN_HEAP_COMPARATOR comparator;
    int size;
} thin_heap;

void insert_to_thin_heap(thin_heap *,thin_node*);

thin_node *extract_min_thin_heap(thin_heap *);