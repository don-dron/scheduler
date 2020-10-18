#include <scheduler/scheduler.h>
#include <structures/rb_tree.h>

struct scheduler_manager
{
    struct rb_tree* tree;
    spinlock lock;
};

typedef struct fiber_node
{
    struct rb_tree_node rb_node;
    fiber *fib;
} fiber_node;