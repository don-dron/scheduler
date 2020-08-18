#pragma once

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include <scheduler/context.h>

struct coroutine;

typedef struct coroutine
{
    void (*routine)();
    execution_context routine_context;
    execution_context caller_context;
    struct coroutine *external_routine;
    volatile int complete;
    void *stack;
} coroutine;

static thread_local volatile coroutine *current_coroutine = NULL;

void suspend();
void resume(coroutine *this);

coroutine create_coroutine(void (*routine)());

void switch_to_caller(coroutine *coroutine);
void setup(coroutine *coroutine, void (*Trampoline)());

void free_coroutine(coroutine *coroutine);