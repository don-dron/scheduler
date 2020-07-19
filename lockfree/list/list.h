#pragma once

#include <stdlib.h>
#include "../locks/spinlock/spinlock.h"

struct list_node;
typedef struct list_node list_node;

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

typedef struct list list;

static list *create_list()
{
    list *lst = (list *)malloc(sizeof(list));
    lst->start = 0;
    lst->end = 0;
    lst->size = 0;

    spinlock lock;
    lst->lock = lock;
    lst->lock.lock = 0;

    return lst;
}

static void push_back(list *lst, list_node *node)
{
    lock_spinlock(&lst->lock);

    if (lst->size == 0)
    {
        node->next = 0;
        node->prev = 0;

        lst->start = node;
        lst->end = node;

        lst->size++;
    }
    else
    {
        lst->end->next = node;
        node->prev = lst->end;
        lst->end = node;
        node->next = 0;

        lst->size++;
    }

    unlock_spinlock(&lst->lock);
}

static void push_front(list *lst, list_node *node)
{
    lock_spinlock(&lst->lock);
    if (lst->size == 0)
    {
        node->next = 0;
        node->prev = 0;

        lst->start = node;
        lst->end = node;

        lst->size++;
    }
    else
    {
        lst->start->prev = node;
        node->next = lst->start;
        lst->start = node;
        node->prev = 0;

        lst->size++;
    }
    unlock_spinlock(&lst->lock);
}

static list_node *pop_back(list *lst)
{
    lock_spinlock(&lst->lock);
    if (lst->size == 0)
    {
        unlock_spinlock(&lst->lock);
        return 0;
    }
    else
    {
        list_node *result = lst->end;
        lst->end = result->prev;

        if (lst->end != 0)
        {
            lst->end->next = 0;
        }
        else
        {
            lst->start = 0;
        }

        lst->size--;
        unlock_spinlock(&lst->lock);
        return result;
    }
}

static list_node *pop_front(list *lst)
{
    lock_spinlock(&lst->lock);
    if (lst->size == 0)
    {
        unlock_spinlock(&lst->lock);
        return 0;
    }
    else
    {
        list_node *result = lst->start;
        lst->start = result->next;

        if (lst->start != 0)
        {
            lst->start->prev = 0;
        }
        else
        {
            lst->end = 0;
        }

        lst->size--;
        unlock_spinlock(&lst->lock);
        return result;
    }
}

static void free_list(list *lst)
{
    lock_spinlock(&lst->lock);

    list_node *current = lst->start;

    while (current != 0)
    {
        list_node *tmp = current->next;
        free(current);
        current = tmp;
    }

    unlock_spinlock(&lst->lock);

    free(lst);
}