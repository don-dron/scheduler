#pragma once

#include <threads.h>

#include <scheduler/context.h>

enum fiber_state
{
    starting,
    runnable,
    running,
    suspended,
    terminated
};

typedef enum fiber_state fiber_state;
typedef void (*fiber_routine)();

struct fiber
{
    // Scheduler* scheduler;
    void *stack;
    execution_context external_context;
    execution_context context;
    fiber_state state;
    fiber_routine routine;
    unsigned long id;
};

typedef struct fiber fiber;
extern thread_local fiber *current_fiber;

fiber *create_fiber(fiber_routine routine);
void setup_trampoline(fiber *fiber);
