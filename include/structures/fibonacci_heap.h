#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef int (*fibonacci_key_cmp_t)(void *k1, void *k2);
typedef void *(*fibonacci_key_min_t)();
typedef void (*fibonacci_key_pr_t)(void *key);

struct fibonacci_heap_node
{
    struct fibonacci_heap_node *parent;
    struct fibonacci_heap_node *left;
    struct fibonacci_heap_node *right;
    struct fibonacci_heap_node *child;
    int if_lost_child;
    size_t degree;
    void *key;
};

struct fibonacci_heap
{
    struct fibonacci_heap_node *min;
    size_t n;
    fibonacci_key_cmp_t key_cmp;
    fibonacci_key_min_t key_min;
    fibonacci_key_pr_t key_pr;
};

struct fibonacci_heap *fibonacci_heap_alloc(fibonacci_key_cmp_t key_cmp, fibonacci_key_min_t key_min, fibonacci_key_pr_t key_pr);
void fibonacci_heap_free(struct fibonacci_heap *fh);
struct fibonacci_heap *fibonacci_heap_union(struct fibonacci_heap *fh_1, struct fibonacci_heap *fh_2);

void insert_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node);
struct fibonacci_heap_node *get_min_node(struct fibonacci_heap *fh);
struct fibonacci_heap_node *extract_min_node(struct fibonacci_heap *fh);
void *decrease_key_by_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node, void *key);
void delete_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node);
struct fibonacci_heap_node *search_key(struct fibonacci_heap *fh, void *key);
void pr(struct fibonacci_heap *fh);

struct fibonacci_heap_node *fibonacci_heap_node_alloc();
