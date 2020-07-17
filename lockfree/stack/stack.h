#pragma once
#include <stdlib.h>
#include <unistd.h>

struct Node;
struct Head;
struct LockFreeStack;

typedef struct Node Node;
typedef struct Head Head;
typedef struct LockFreeStack LockFreeStack;

struct Node
{
    int data;
    size_t list_mutex;
    Node *next;
};
struct Head
{
    size_t list_mutex;
    Node *next;
};
struct LockFreeStack
{
    size_t size;
    Head *head;
};

Node *NewNode(int item, Node *next_node);

Head *NewHead(Node *next_node);

LockFreeStack *NewStack();

void Push(LockFreeStack *stack, int item);

void Delete_nodes(LockFreeStack *stack);

int Pop(LockFreeStack *stack, int *item);

void FreeStack(LockFreeStack *stack);