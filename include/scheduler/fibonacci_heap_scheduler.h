#include <scheduler/scheduler.h>
#include <structures/fibonacci_heap.h>

struct scheduler_manager
{
    struct fibonacci_heap* heap;
    spinlock lock;
};