#include <scheduler/rb_tree_scheduler.h>

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

    manager->tree = (struct rb_tree *)calloc(1, sizeof(struct rb_tree));
    rb_tree_new(manager->tree, cmp);
    return 0;
}

fiber *get_from_pool()
{
    lock_spinlock(&current_scheduler->manager->lock);
    struct rb_tree *tree = current_scheduler->manager->tree;
    fiber_node *fib_node = NULL;

    if (RB_OK != rb_tree_get_rightmost(tree, (struct rb_tree_node **)&fib_node))
    {
        unlock_spinlock(&current_scheduler->manager->lock);
        return NULL;
    }

    if (fib_node == NULL)
    {
        unlock_spinlock(&current_scheduler->manager->lock);
        return NULL;
    }

    rb_tree_remove(tree, tree->rightmost);

    unlock_spinlock(&current_scheduler->manager->lock);
    fiber *res = fib_node->fib;
    // printf("%llu %d\n", fib_node->fib->vruntime, tree->size);
    free(fib_node);
    return res;
}

void return_to_pool(scheduler *sched, fiber *fib)
{
    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    lock_spinlock(&sched->manager->lock);
    rb_tree_insert(sched->manager->tree, (struct rb_tree_node *)fib_node, (struct rb_tree_node *)fib_node);
    unlock_spinlock(&sched->manager->lock);
}

int free_scheduler_manager(scheduler *sched)
{
    scheduler_manager *manager = sched->manager;

    rb_tree_destroy(manager->tree);
    free(manager->tree);
    free(sched->manager);
    return 0;
}