#include "stack.h"

Node *NewNode(int item, Node *next_node)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->next = next_node;
    node->data = item;
    node->list_mutex = 0;
    return node;
}

Head *NewHead(Node *next_node)
{
    Head *head = (Head *)malloc(sizeof(Head));
    head->next = next_node;
    head->list_mutex = 0;
    return head;
}

LockFreeStack *NewStack()
{
    LockFreeStack *lockFreeStack = (LockFreeStack *)malloc(sizeof(LockFreeStack));
    lockFreeStack->head = NewHead(0);
    lockFreeStack->head->next = 0;
    return lockFreeStack;
}

void Push(LockFreeStack *stack, int item)
{
    Node *tb, *oldhead;
    tb = NewNode(item, 0);

    oldhead = stack->head->next;
    tb->next = oldhead;

    while (!__atomic_compare_exchange(&(stack->head->next), &oldhead, &tb, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
    {
        usleep(1);
        oldhead = stack->head->next;
        tb->next = oldhead;
    }
}

void Delete_nodes(LockFreeStack *stack)
{
    Node *top = stack->head->next;
    Node *curr_top = top;
    while (top != 0)
    {
        curr_top = top;
        top = top->next;
        free(curr_top);
    }
}

int Pop(LockFreeStack *stack, int *item)
{
    Node *current;
    int flag = 0;

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
        flag = 1;
        *item = current->data;
        free(current);
    }

    while (!__sync_bool_compare_and_swap(&(stack->head->list_mutex), 1, 0))
    {
        usleep(1);
    }

    return flag;
}

void FreeStack(LockFreeStack *stack)
{
    Delete_nodes(stack);
    free(stack->head);
    free(stack);
}