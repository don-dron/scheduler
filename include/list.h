#pragma once

#include <stdlib.h>

#include <spinlock.h>

struct list_node;
typedef struct list_node list_node;

struct list;
typedef struct list list;

struct list_node
{
    list_node *prev;
    list_node *next;
};

struct list
{
    list_node *start;
    list_node *end;
    spinlock lock;
    size_t size;
};

list *create_list();

void list_push_back(list *lst, list_node *node);

void list_push_front(list *lst, list_node *node);

list_node *list_pop_back(list *lst);

list_node *list_pop_front(list *lst);

void free_list(list *lst);