#include <scheduler/splay_tree_scheduler.h>

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

    manager->tree = splay_tree_new_tree((splay_tree_cmp)cmp);

    return 0;
}

fiber *get_from_pool()
{
    splay_tree *tree = current_scheduler->manager->tree;

    lock_spinlock(&current_scheduler->manager->lock);
    splay_node *node = splay_tree_first(tree);
    if (node)
    {
        splay_tree_delete_hint(tree, node);
        unlock_spinlock(&current_scheduler->manager->lock);
        fiber *res = ((fiber_node *)node->value)->fib;
        return res;
    }
    else
    {
        unlock_spinlock(&current_scheduler->manager->lock);
        return NULL;
    }
}

void return_to_pool(scheduler *sched, fiber *fib)
{
    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    lock_spinlock(&sched->manager->lock);
    splay_tree_insert(sched->manager->tree, fib_node);
    unlock_spinlock(&sched->manager->lock);
}

int free_scheduler_manager(scheduler *sched)
{
    // scheduler_manager *manager = sched->manager;

    // Free TODO
    free(sched->manager);
    return 0;
}