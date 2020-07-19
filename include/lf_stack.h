#pragma once

#include <stdlib.h>
#include <unistd.h>

struct stack_node;
struct head;
struct lf_stack;

typedef struct stack_node stack_node;
typedef struct head head;
typedef struct lf_stack lf_stack;

struct stack_node
{
    stack_node *next;
    size_t list_mutex;
};
struct head
{
    stack_node *next;
    size_t list_mutex;
};
struct lf_stack
{
    head *head;
    size_t size;
};

stack_node *create_node(stack_node *next_node);

head *create_head(stack_node *next_node);

lf_stack *create_stack();

void push(lf_stack *stack, stack_node *node);

void free_nodes(lf_stack *stack);

stack_node* pop(lf_stack *stack);

void free_stack(lf_stack *stack);