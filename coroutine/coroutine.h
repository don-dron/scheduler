#pragma once

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include "../context/context.h"
#include <threads.h>

#if defined USE_VALGRIND
#include <valgrind/valgrind.h>
#endif

struct coroutine;
typedef struct coroutine coroutine;

struct coroutine
{
    void (*routine)();
    execution_context routine_context;
    execution_context caller_context;
    coroutine *external_routine;
    volatile int complete;
    void *stack;
};

thread_local volatile static coroutine *current_coroutine = NULL;
void suspend();
void resume(coroutine *this);

coroutine *create_coroutine_on_heap(void (*routine)());
coroutine create_coroutine_on_stack(void (*routine)());

void switch_to_caller(coroutine *coroutine);
void setup(coroutine *coroutine, void (*Trampoline)());

void free_coroutine_on_stack(coroutine *coroutine);
void free_coroutine_on_heap(coroutine *coroutine);