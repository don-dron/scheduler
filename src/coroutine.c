#pragma once

#include <coroutine.h>

void suspend()
{
    coroutine *next_coroutine = current_coroutine;
    current_coroutine = next_coroutine->external_routine;
    switch_to_caller(next_coroutine);
}

void trampoline()
{
    current_coroutine->routine();

    current_coroutine->complete = 1;

    suspend();
}

void setup(coroutine *coroutine_, void (*trampoline)())
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

    // Machine word size, usually 8 bytes(x86)
    stackBuilder.word_size = sizeof(void *);

    // For rbp register(shift 16 bytes)
    AlignNextPush(&stackBuilder, 16);
    // Allocate stack on allocated memory
    Allocate(&stackBuilder, sizeof(stack_saved_context));

    stack_saved_context *saved_context = (stack_saved_context *)stackBuilder.top;

    // Rsp - stack pointer - new stack saved context - pointer to top allocated stack.
    // Rip - instruction pointer to trampoline function, after switch context automatically executed trampoline function.

    saved_context->rip = (void *)trampoline;
    coroutine_->routine_context.rsp = saved_context;

    // Save allocated memory pointer
    coroutine_->stack = start;
}

coroutine create_coroutine_on_stack(void (*routine)())
{
    coroutine new_coroutine;

    new_coroutine.routine = routine;
    new_coroutine.complete = 0;

    setup(&new_coroutine, trampoline);

    new_coroutine.external_routine = current_coroutine;
    return new_coroutine;
}

coroutine *create_coroutine_on_heap(void (*routine)())
{
    coroutine *new_coroutine = (coroutine *)malloc(sizeof(coroutine));

    new_coroutine->routine = routine;
    new_coroutine->complete = 0;

    setup(new_coroutine, trampoline);

    new_coroutine->external_routine = current_coroutine;
    return new_coroutine;
}

void resume(coroutine *coroutine_by_resume)
{
    if (coroutine_by_resume->complete)
    {
        return;
    }

    current_coroutine = coroutine_by_resume;

    switch_context(&coroutine_by_resume->caller_context, &coroutine_by_resume->routine_context);
}

void switch_to_caller(coroutine *coroutine_)
{
    switch_context(&coroutine_->routine_context, &coroutine_->caller_context);
}

void free_coroutine_on_stack(coroutine *coroutine_)
{
    munmap(coroutine_->stack, STACK_SIZE);
}

void free_coroutine_on_heap(coroutine *coroutine_)
{
    munmap(coroutine_->stack, STACK_SIZE);
    free(coroutine_);
}