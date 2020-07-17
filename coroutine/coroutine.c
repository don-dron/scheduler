#include "coroutine.h"

struct Stack
{
    void *memory;
    size_t size;
};

typedef struct Stack Stack;

char *Bottom(Stack *stack);

struct StackBuilder
{
    int kWordSize;
    char *top_;
};

typedef struct StackBuilder StackBuilder;

void AlignNextPush(StackBuilder *builder, size_t alignment)
{
    size_t shift = (size_t)(builder->top_ - builder->kWordSize) % alignment;
    builder->top_ -= shift;
}

void Allocate(StackBuilder *builder, size_t bytes)
{
    builder->top_ -= bytes;
}

void Suspend()
{
    Coroutine *newCoroutine = current;
    current = newCoroutine->external_routine;
    SwitchToCaller(newCoroutine);
}

void Trampoline()
{
    current->routine();

    current->complete = 1;

    Suspend();
}

static size_t PagesToBytes(size_t count)
{
    static const size_t kPageSize = 4096;

    return count * kPageSize;
}

#define VALGRIND_STACK_REGISTER(a, b) (void)0

void Setup(Coroutine *coroutine, void (*trampoline)())
{
    void *start = mmap(/*addr=*/0, /*length=*/STACK_SIZE,
                       /*prot=*/PROT_READ | PROT_WRITE,
                       /*flags=*/MAP_PRIVATE | 0x20,
                       /*fd=*/-1, /*offset=*/0);

    int ret = mprotect(/*addr=*/(void *)(start + PagesToBytes(4)),
                       /*len=*/PagesToBytes(4),
                       /*prot=*/PROT_NONE);

    (void)VALGRIND_STACK_REGISTER(start, start + STACK_SIZE);

    StackBuilder stackBuilder;
    stackBuilder.top_ = start + STACK_SIZE - 1;

    stackBuilder.kWordSize = sizeof(void *);

    AlignNextPush(&stackBuilder, 16);
    Allocate(&stackBuilder, sizeof(StackSavedContext));

    StackSavedContext *saved_context = (StackSavedContext *)stackBuilder.top_;
    saved_context->rip = (void *)trampoline;

    coroutine->routine_context.rsp_ = saved_context;
    coroutine->stack = start;
}

Coroutine NewCoroutineOnStack(void (*routine)())
{
    Coroutine new_coroutine;

    new_coroutine.routine = routine;
    new_coroutine.complete = 0;

    Setup(&new_coroutine, Trampoline);

    new_coroutine.external_routine = current;
    return new_coroutine;
}

Coroutine *NewCoroutineOnHeap(void (*routine)())
{
    Coroutine *new_coroutine = (Coroutine *)malloc(sizeof(Coroutine));

    new_coroutine->routine = routine;
    new_coroutine->complete = 0;

    Setup(new_coroutine, Trampoline);

    new_coroutine->external_routine = current;
    return new_coroutine;
}

void Resume(Coroutine *this)
{
    if (this->complete)
    {
        return;
    }

    current = this;

    SwitchContext(&this->caller_context, &this->routine_context);
}

void SwitchToCaller(Coroutine *coroutine)
{
    SwitchContext(&coroutine->routine_context, &coroutine->caller_context);
}

void FreeCoroutineOnStack(Coroutine *coroutine)
{
    munmap(coroutine->routine_context.stack, STACK_SIZE);
}

void FreeCoroutineOnHeap(Coroutine *coroutine)
{
    munmap(coroutine->routine_context.stack, STACK_SIZE);
    free(coroutine);
}