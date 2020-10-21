#include <scheduler/fibonacci_heap_scheduler.h>

static int cmp(void *lhs, void *rhs)
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

static void *key_min(void)
{
    struct fibonacci_heap_node *node = fibonacci_heap_node_alloc();
    node->key = NULL;
    return node;
}

static void key_pr(void *key)
{
}

int create_scheduler_manager(scheduler *sched)
{
    sched->manager = (scheduler_manager *)malloc(sizeof(scheduler_manager));
    scheduler_manager *manager = sched->manager;
    init_spinlock(&manager->lock);

    manager->heap = fibonacci_heap_alloc((fibonacci_key_cmp_t)cmp, (fibonacci_key_min_t)key_min, (fibonacci_key_pr_t)key_pr);
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
    struct fibonacci_heap_node *node = fibonacci_heap_extract_min_node(heap);
    unlock_spinlock(&current_scheduler->manager->lock);
    asm volatile("mfence" ::
                     : "memory");

    if (node != NULL)
    {      
        fiber* fib = (fiber*)node->key;
        free(node);
        return fib;
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
    struct fibonacci_heap_node *node = fibonacci_heap_node_alloc();
    node->key = fib;
    asm volatile("mfence" ::
                     : "memory");

    lock_spinlock(&sched->manager->lock);
    fibonacci_heap_insert_node(sched->manager->heap, node);
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