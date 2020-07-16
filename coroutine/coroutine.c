#include "coroutine.h"
#include "stdlib.h"

void Suspend()
{
    Coroutine *kek = current;
    current = kek->external_routine;
    SwitchToCaller(kek);
}

void Trampoline()
{
    void (*routine)() = current->routine;
    routine();

    current->complete = 1;

    SwitchToCaller(current);
}

Coroutine *NewCoroutine(void (*routine)())
{
    Coroutine *new_coroutine = (Coroutine *)malloc(sizeof(Coroutine));
    new_coroutine->routine_context = (ExecutionContext *)malloc(sizeof(ExecutionContext));
    new_coroutine->caller_context = (ExecutionContext *)malloc(sizeof(ExecutionContext));
    new_coroutine->routine = routine;
    new_coroutine->complete = 0;
    new_coroutine->stackMemory = Setup(Trampoline, new_coroutine->routine_context);

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
    SwitchContext(this->caller_context, this->routine_context);
}

void SwitchToCaller(Coroutine *coroutine)
{
    SwitchContext(coroutine->routine_context, coroutine->caller_context);
}

void FreeCoroutine(Coroutine *coroutine)
{
    free(coroutine->stackMemory);
    free(coroutine->routine_context);
    free(coroutine->caller_context);
    free(coroutine);
}