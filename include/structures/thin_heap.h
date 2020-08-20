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
} thin_heap;

int is_thin_node(thin_node *node);

thin_heap *create_thin_heap(THIN_HEAP_COMPARATOR comparator);

void insert_to_thin_heap(thin_heap *heap, thin_node *node);

thin_node *get_thin_heap_min(thin_heap *heap);

thin_heap *merge_thin_heaps(thin_heap *first_heap, thin_heap *second_heap);

thin_node *extract_min_thin_heap(thin_heap *heap);

void decrease_key_thin_heap(thin_heap *heap, thin_node *node, DECREASE_KEY_FUNCTION func);

void delete_thin_heap(thin_heap *heap, thin_node *node, DECREASE_KEY_FUNCTION func);