#pragma once

#include <structures/lf_stack.h>

stack_node *create_node(stack_node *next_node)
{
    stack_node *created_node = (stack_node *)malloc(sizeof(stack_node));
    created_node->next = next_node;
    created_node->list_mutex = 0;
    return created_node;
}

head *create_head(stack_node *next_node)
{
    head *created_head = (head *)malloc(sizeof(head));
    created_head->next = next_node;
    created_head->list_mutex = 0;
    return created_head;
}

lf_stack *create_stack()
{
    lf_stack *lockFreeStack = (lf_stack *)malloc(sizeof(lf_stack));
    lockFreeStack->head = create_head(0);
    lockFreeStack->head->next = 0;
    return lockFreeStack;
}

void push(lf_stack *stack, stack_node *node)
{
    stack_node *tb, *oldhead;
    tb = node;

    oldhead = stack->head->next;
    tb->next = oldhead;

    while (!__atomic_compare_exchange(&(stack->head->next), &oldhead, &tb, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
    {
        usleep(1);
        oldhead = stack->head->next;
        tb->next = oldhead;
    }

    __atomic_fetch_add(&stack->size, 1, __ATOMIC_SEQ_CST);
}

void free_nodes(lf_stack *stack)
{
    stack_node *top = stack->head->next;
    stack_node *curr_top = top;
    while (top != 0)
    {
        curr_top = top;
        top = top->next;
        free(curr_top);
    }
}

stack_node* pop(lf_stack *stack)
{
    stack_node *current;

    while (!__sync_bool_compare_and_swap(&(stack->head->list_mutex), 0, 1))
    {
        usleep(1);
    }

    current = stack->head->next;

    while (current && !__atomic_compare_exchange(&(stack->head->next), &current, &(current->next), 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
    {
        usleep(1);
        current = stack->head->next;
    }

    if (current)
    {
        __atomic_fetch_sub(&stack->size, 1, __ATOMIC_SEQ_CST);
    }

    while (!__sync_bool_compare_and_swap(&(stack->head->list_mutex), 1, 0))
    {
        usleep(1);
    }

    return current;
}

void free_stack(lf_stack *stack)
{
    free_nodes(stack);
    free(stack->head);
    free(stack);
}