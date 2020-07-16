#pragma once

#include <stdio.h>
#include "../context/context.h"

#ifndef COROUTINE
#define COROUTINE
 
struct Coroutine;
typedef struct Coroutine Coroutine;

struct Coroutine
{
    void (*routine)();
    ExecutionContext *routine_context;
    ExecutionContext *caller_context;
    Coroutine *external_routine;
    int complete;
};

static Coroutine *current = 0;

Coroutine* NewCoroutine(void (*routine)());
void SwitchToCaller(Coroutine *coroutine);

#endif