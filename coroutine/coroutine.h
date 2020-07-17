#pragma once

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include "../context/context.h"
#include <threads.h>
#if defined USE_VALGRIND
#include <valgrind/valgrind.h>
#endif

#ifndef COROUTINE
#define COROUTINE

static const size_t STACK_SIZE = 64 * 1024;
struct Coroutine;
typedef struct Coroutine Coroutine;

struct Coroutine
{
    void (*routine)();
    ExecutionContext routine_context;
    ExecutionContext caller_context;
    Coroutine *external_routine;
    volatile int complete;
    void *stack;
};

thread_local volatile static Coroutine *current = NULL;
void Suspend();
void Resume(Coroutine *this);

Coroutine NewCoroutineOnStack(void (*routine)());

void SwitchToCaller(Coroutine *coroutine);
void Setup(Coroutine *coroutine, void (*Trampoline)());

void FreeCoroutineOnStack(Coroutine *coroutine);
void FreeCoroutineOnHeap(Coroutine *coroutine);

#endif