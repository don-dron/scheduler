#pragma once

#include <threads.h>

#include <scheduler/context.h>
#include <locks/atomics.h>

enum fiber_state
{
    starting,
    runnable,
    running,
    sleeping,
    suspended,
    terminated
};

typedef enum fiber_state fiber_state;
typedef void (*fiber_routine)();

typedef struct fiber
{
    // Scheduler* scheduler;
    void *stack;
    execution_context external_context;
    execution_context context;
    fiber_state state;
    fiber_routine routine;
    struct fiber *parent;
    unsigned long id;
    unsigned long wakeup;
} fiber;

extern thread_local fiber *current_fiber;
extern unsigned long id;

fiber *create_fiber(fiber_routine routine);
void free_fiber(fiber *fiber_);
void setup_trampoline(fiber *fiber);
