#pragma once

#include <stdlib.h>
#include <unistd.h>

struct lf_stack_node;
struct lf_stack_head;
struct lf_stack;

typedef struct lf_stack_node lf_stack_node;
typedef struct lf_stack_head lf_stack_head;
typedef struct lf_stack lf_stack;

struct lf_stack_node
{
    lf_stack_node *next;
    size_t list_mutex;
};
struct lf_stack_head
{
    lf_stack_node *next;
    size_t list_mutex;
};
struct lf_stack
{
    lf_stack_head *head;
    size_t size;
};

lf_stack_node *create_lf_stack_node(lf_stack_node *next_node);

lf_stack_head *create_lf_stack_head(lf_stack_node *next_node);

lf_stack *create_lf_stack();

void push_lf_stack(lf_stack *stack, lf_stack_node *node);

void free_lf_stack_nodes(lf_stack *stack);

lf_stack_node* pop_lf_stack(lf_stack *stack);

void free_lf_stack(lf_stack *stack);