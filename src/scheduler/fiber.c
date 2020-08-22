#include <scheduler/fiber.h>

thread_local fiber *current_fiber = NULL;

unsigned long id = 0;

static unsigned long generate_id()
{
    return inc(&id);
}

static void fiber_trampoline()
{
    fiber *temp = current_fiber;

    temp->state = running;

    // Unlock after lock in run_task
    unlock_spinlock(&temp->lock);

    temp->routine(temp->args);

    // Lock for swtich context, unlocked in run_task
    lock_spinlock(&temp->lock);

    if (temp != current_fiber)
    {
        printf("Wrong state\n");
        exit(1);
        return;
    }

    if (temp->state == running)
    {
        temp->state = terminated;

        // To run task
        switch_context(&temp->context, &temp->external_context);

        // Unreachable
        printf("Wrong state\n");
        exit(1);
    }
    else
    {
        printf("Wrong state\n");
        exit(1);
    }
}

fiber *create_fiber(fiber_routine routine, void *args)
{
    fiber *new_fiber = (fiber *)malloc(sizeof(fiber));

    new_fiber->id = generate_id();
    new_fiber->routine = routine;
    new_fiber->state = starting;
    new_fiber->parent = current_fiber;
    new_fiber->args = args;

    init_spinlock(&new_fiber->lock);

    setup_trampoline(new_fiber);

    return new_fiber;
}

void free_fiber(fiber *fiber_)
{
    munmap(fiber_->stack, STACK_SIZE);
    free(fiber_);
}

void setup_trampoline(fiber *new_fiber)
{
    // Allocate memory for stack and context
    void *start = mmap(/*addr=*/0, /*length=*/STACK_SIZE,
                       /*prot=*/PROT_READ | PROT_WRITE,
                       /*flags=*/MAP_PRIVATE | 0x20,
                       /*fd=*/-1, /*offset=*/0);

    //int ret =
    mprotect(/*addr=*/(void *)((size_t)start + pages_to_bytes(4)),
             /*len=*/pages_to_bytes(4),
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
    stackBuilder.top = (char *)((size_t)start + STACK_SIZE - 1);

    // Machine word size, usually 8 bytes
    stackBuilder.word_size = sizeof(void *);

    // For rbp register(shift 16 bytes)
    align_next_push(&stackBuilder, 16);
    // Allocate stack on allocated memory
    allocate(&stackBuilder, sizeof(stack_saved_context));

    stack_saved_context *saved_context = (stack_saved_context *)stackBuilder.top;

    // Rsp - stack pointer - new stack saved context - pointer to top allocated stack.
    // Rip - instruction pointer to trampoline function, after switch context automatically executed trampoline function.

    saved_context->rip = (void *)fiber_trampoline;
    new_fiber->context.rsp = saved_context;

    // Save allocated memory pointer
    new_fiber->stack = start;
}