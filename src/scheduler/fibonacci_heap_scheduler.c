#include <scheduler/thin_heap_scheduler.h>

static int cmp(const void *lhs, const void *rhs)
{
    fiber *first = (fiber *)lhs;
    fiber *second = (fiber *)rhs;

    if (lhs == NULL)
    {
        return -1;
    }

    if (rhs == NULL)
    {
        return 11;
    }

    if (first->level < second->level)
    {
        return -1;
    }
    else if (first->level > second->level)
    {
        return 1;
    }
    else
    {
        if (first->vruntime < second->vruntime)
        {
            return -1;
        }
        else if (first->vruntime > second->vruntime)
        {
            return 1;
        }
        else
        {
            return ((unsigned long)first) > ((unsigned long)second);
        }
    }
}

static void swp(struct thin_heap_node *first, struct thin_heap_node *second)
{
    fiber_node *f = (fiber_node *)first;
    fiber_node *s = (fiber_node *)second;
    fiber *temp;

    temp = f->fib;
    f->fib = s->fib;
    s->fib = temp;
}

void *key_min()
{
    return NULL;
}

void key_pr(void *key)
{
}

int create_scheduler_manager(scheduler *sched)
{
    sched->manager = (scheduler_manager *)malloc(sizeof(scheduler_manager));
    scheduler_manager *manager = sched->manager;
    init_spinlock(&manager->lock);

    manager->heap = fibonacci_heap_alloc(manager->heap, cmp, key_min, key_pr);
    asm volatile("mfence" ::
                     : "memory");
    return 0;
}

fiber *get_from_pool()
{
    asm volatile("mfence" ::
                     : "memory");
    struct fibonacci_heap *heap = current_scheduler->manager->heap;

    lock_spinlock(&current_scheduler->manager->lock);
    fiber *node = extract_min_node(heap);
    unlock_spinlock(&current_scheduler->manager->lock);
    asm volatile("mfence" ::
                     : "memory");

    if (node)
    {
        return node;
    }
    else
    {
        return NULL;
    }
}

void return_to_pool(scheduler *sched, fiber *fib)
{
    asm volatile("mfence" ::
                     : "memory");
    struct fibonacci_heap_node *node = (struct fibonacci_heap_node *)malloc(sizeof(struct fibonacci_heap_node));
    node->key = fib;
    asm volatile("mfence" ::
                     : "memory");

    lock_spinlock(&sched->manager->lock);
    insert_node(sched->manager->heap, node);
    unlock_spinlock(&sched->manager->lock);
    asm volatile("mfence" ::
                     : "memory");
}

int free_scheduler_manager(scheduler *sched)
{
    scheduler_manager *manager = sched->manager;

    free(manager->heap);

    free(sched->manager);
    return 0;
}