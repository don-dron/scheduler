#pragma once

#include <fiber.h>

extern thread_local fiber *current_fiber = 0;

unsigned long GenerateId()
{
    return 1;
}

fiber *create_fiber(fiber_routine routine)
{
    fiber *new_fiber = (fiber *)malloc(sizeof(fiber));

    new_fiber->id = GenerateId();
    new_fiber->routine = routine;
    new_fiber->state = starting;

    setup_trampoline(new_fiber);

    return new_fiber;
}

void fiber_trampoline()
{
    fiber *self = current_fiber;

    self->state = running;

    self->routine();

    current_fiber->state = terminated;

    switch_context(&current_fiber->context, &current_fiber->external_context);
}

void setup_trampoline(fiber *new_fiber)
{
    // Allocate memory for stack and context
    void *start = mmap(/*addr=*/0, /*length=*/STACK_SIZE,
                       /*prot=*/PROT_READ | PROT_WRITE,
                       /*flags=*/MAP_PRIVATE | 0x20,
                       /*fd=*/-1, /*offset=*/0);

    int ret = mprotect(/*addr=*/(void *)(start + PagesToBytes(4)),
                       /*len=*/PagesToBytes(4),
                       /*prot=*/PROT_NONE);

    stack_builder stackBuilder;
    // Set top stack address
    //
    // Programm heap
    //
    // 0xfffff
    //   ^
    //   |
    //   |
    //   ^
    //   |------------------ top = start + stack_size - 1   - Because stack grows downward
    //   |                                                                  |
    //   |                   Coroutine stack memory.                        |
    //   |                                                                  |
    //   |------------------ start                                          v
    //   ^
    //   |
    //   |
    //   0
    stackBuilder.top = start + STACK_SIZE - 1;

    // Machine word size, usually 8 bytes
    stackBuilder.word_size = sizeof(void *);

    // For rbp register(shift 16 bytes)
    AlignNextPush(&stackBuilder, 16);
    // Allocate stack on allocated memory
    Allocate(&stackBuilder, sizeof(stack_saved_context));

    stack_saved_context *saved_context = (stack_saved_context *)stackBuilder.top;

    // Rsp - stack pointer - new stack saved context - pointer to top allocated stack.
    // Rip - instruction pointer to trampoline function, after switch context automatically executed trampoline function.

    saved_context->rip = (void *)fiber_trampoline;
    new_fiber->context.rsp = saved_context;

    // Save allocated memory pointer
    new_fiber->stack = start;
}