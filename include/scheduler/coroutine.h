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

void suspend(void);
void resume(coroutine *this);

coroutine create_coroutine(void (*routine)(void *), void *args);

void switch_to_caller(coroutine *coroutine);
void setup(coroutine *coroutine, void (*Trampoline)());

void free_coroutine(coroutine *coroutine);