#include <scheduler/rb_tree_scheduler.h>

static int cmp(const void *lhs, const void *rhs)
{
    // fiber_node *first = (fiber_node *)lhs;
    // fiber_node *second = (fiber_node *)rhs;
    return -1;
}

int create_scheduler_manager(scheduler *sched)
{
    printf("1asdsadas234\n");
    sched->manager = (scheduler_manager *)malloc(sizeof(scheduler_manager));
    scheduler_manager *manager = sched->manager;

    manager->tree = (struct rb_tree *)calloc(1, sizeof(struct rb_tree));
    rb_tree_new(manager->tree, cmp);
    return 0;
}

fiber *get_from_pool()
{
    printf("1asdsadas234\n");
    struct rb_tree *tree = current_scheduler->manager->tree;
    fiber_node *fib_node = (fiber_node *)tree->rightmost;
    rb_tree_remove(tree, tree->rightmost);

    if (fib_node)
    {
        fiber *res = fib_node->fib;
        free(fib_node);
        return res;
    }
    else
    {
        return NULL;
    }
}

void return_to_pool(scheduler *sched, fiber *fib)
{
    printf("1asdsadas234\n");
    fiber_node *fib_node = (fiber_node *)malloc(sizeof(fiber_node));
    fib_node->fib = fib;
    rb_tree_insert(sched->manager->tree, (struct rb_tree_node *)fib_node, (struct rb_tree_node *)fib_node);
}

int free_scheduler_manager(scheduler *sched)
{
    printf("1asdsadas234\n");
    scheduler_manager *manager = sched->manager;

    rb_tree_destroy(manager->tree);
    free(manager->tree);
    free(sched->manager);
    return 0;
}