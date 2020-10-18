#include <scheduler/thin_heap_scheduler.h>

static int cmp(const void *lhs, const void *rhs)
{
    fiber_node *first = (fiber_node *)lhs;
    fiber_node *second = (fiber_node *)rhs;

    if (first->fib->level < second->fib->level)
    {
        return 1;
    }
    else if (first->fib->level > second->fib->level)
    {
        return -1;
    }
    else
    {
        if (first->fib->vruntime < second->fib->vruntime)
        {
            return 1;
        }
        else if (first->fib->vruntime > second->fib->vruntime)
        {
            return -1;
        }
        else
        {
            if (((unsigned long)first) > ((unsigned long)second))
            {
                return 1;
            }
            else if (((unsigned long)first) < ((unsigned long)second))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

int create_scheduler_manager(scheduler *sched)
{
    sched->manager = (scheduler_manager *)malloc(sizeof(scheduler_manager));
    scheduler_manager *manager = sched->manager;
    init_spinlock(&manager->lock);

    manager->heap = (thin_heap *)malloc(sizeof(thin_heap));
    manager->heap->comparator = (THIN_HEAP_COMPARATOR)cmp;
    manager->heap->first = NULL;
    manager->heap->last = NULL;
    manager->heap->size = 0;
    asm volatile("mfence":::"memory");
    return 0;
}

fiber *get_from_pool()
{
    asm volatile("mfence":::"memory");
    thin_heap *heap = current_scheduler->manager->heap;

    lock_spinlock(&current_scheduler->manager->lock);
    fiber_node *node = (fiber_node *)extract_min_thin_heap(heap);
    unlock_spinlock(&current_scheduler->manager->lock);
    asm volatile("mfence":::"memory");
    
    if (node)
    {
        fiber *res = node->fib;
        free(node);
        return res;
    }
    else
    {
        return NULL;
    }
}

void return_to_pool(scheduler *sched, fiber *fib)
{
    asm volatile("mfence":::"memory");
    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    fib_node->lst_node.rank = 0;
    fib_node->lst_node.child = NULL;
    fib_node->lst_node.right = NULL;
    fib_node->lst_node.left = NULL;
    asm volatile("mfence":::"memory");

    lock_spinlock(&sched->manager->lock);
    insert_to_thin_heap(sched->manager->heap, (thin_node *)fib_node);
    unlock_spinlock(&sched->manager->lock);
    asm volatile("mfence":::"memory");
}

int free_scheduler_manager(scheduler *sched)
{
    scheduler_manager *manager = sched->manager;

    free(manager->heap);

    free(sched->manager);
    return 0;
}