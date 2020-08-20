#pragma once

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include <scheduler/context.h>

struct coroutine;

typedef struct coroutine
{
    void *args;
    void (*routine)(void *);
    execution_context routine_context;
    execution_context caller_context;
    struct coroutine *external_routine;
    volatile int complete;
    void *stack;
} coroutine;

extern thread_local coroutine *current_coroutine;

int create_coroutine(coroutine *new_coroutine, void (*routine)(void *), void *args);
void suspend(void);
void resume(coroutine *this);
void switch_to_caller(coroutine *coroutine);
int free_coroutine(coroutine *coroutine);