#pragma once

#include <stdlib.h>
#include <assert.h>

#define RB_ASSERT_ARG(x)       \
    do                         \
    {                          \
        if (RB_UNLIKELY(!(x))) \
        {                      \
            assert(#x && 0);   \
            return RB_BAD_ARG; \
        }                      \
    } while (0)

#define RB_UNLIKELY(x) __builtin_expect(!!(x), 0)

struct rb_tree_node
{
    struct rb_tree_node *left;
    struct rb_tree_node *right;
    struct rb_tree_node *parent;
    const void *key;
    int color;
};

typedef int (*rb_cmp_func_t)(const void *lhs, const void *rhs);

typedef int (*rb_cmp_func_ex_t)(void *state, const void *lhs, const void *rhs);

struct rb_tree
{
    struct rb_tree_node *root;
    rb_cmp_func_ex_t compare;

    struct rb_tree_node *rightmost;
    void *state;
};

typedef int rb_result_t;

#define RB_OK 0x0
#define RB_NOT_FOUND 0x1
#define RB_BAD_ARG 0x2
#define RB_DUPLICATE 0x3

#define RB_CONTAINER_OF(x, type, memb)                         \
    ({                                                         \
        const __typeof__(((type *)0)->memb) *__member = (x);   \
        (type *)((char *)__member - __offsetof__(type, memb)); \
    })

rb_result_t rb_tree_new_ex(struct rb_tree *tree, rb_cmp_func_ex_t compare, void *state);

rb_result_t rb_tree_new(struct rb_tree *tree,
                        rb_cmp_func_t compare);

rb_result_t rb_tree_destroy(struct rb_tree *tree);

rb_result_t rb_tree_empty(struct rb_tree *tree, int *is_empty);

rb_result_t rb_tree_find(struct rb_tree *tree,
                         const void *key,
                         struct rb_tree_node **value);

rb_result_t rb_tree_insert(struct rb_tree *tree,
                           const void *key,
                           struct rb_tree_node *node);

rb_result_t rb_tree_remove(struct rb_tree *tree,
                           struct rb_tree_node *node);

rb_result_t rb_tree_find_or_insert(struct rb_tree *tree,
                                   void *key,
                                   struct rb_tree_node *new_candidate,
                                   struct rb_tree_node **value);

rb_result_t rb_tree_find_or_insert(struct rb_tree *tree,
                                   void *key,
                                   struct rb_tree_node *new_candidate,
                                   struct rb_tree_node **value);

static inline rb_result_t rb_tree_get_rightmost(struct rb_tree *tree,
                                                struct rb_tree_node **rightmost)
{
    if ((NULL == tree) || (NULL == rightmost))
    {
        return RB_BAD_ARG;
    }

    *rightmost = tree->rightmost;

    return RB_OK;
}

static inline rb_result_t __rb_tree_find_minimum(struct rb_tree_node *root,
                                                 struct rb_tree_node **min)
{
    struct rb_tree_node *x = root;

    while (x->left != NULL)
    {
        x = x->left;
    }

    *min = x;

    return RB_OK;
}

static inline rb_result_t __rb_tree_find_maximum(struct rb_tree_node *root,
                                                 struct rb_tree_node **max)
{
    struct rb_tree_node *x = root;

    while (x->right != NULL)
    {
        x = x->right;
    }

    *max = x;

    return RB_OK;
}

static inline rb_result_t rb_tree_find_successor(struct rb_tree *tree,
                                                 struct rb_tree_node *node,
                                                 struct rb_tree_node **successor)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(node != NULL);
    RB_ASSERT_ARG(successor != NULL);

    struct rb_tree_node *x = node;

    if (x->right != NULL)
    {
        __rb_tree_find_minimum(x->right, successor);
        goto done;
    }

    struct rb_tree_node *y = x->parent;

    while (y != NULL && (x == y->right))
    {
        x = y;
        y = y->parent;
    }

    *successor = y;

done:
    return ret;
}

static inline rb_result_t rb_tree_find_predecessor(struct rb_tree *tree,
                                                   struct rb_tree_node *node,
                                                   struct rb_tree_node **pred)
{
    rb_result_t ret = RB_OK;
    struct rb_tree_node *x = node;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(node != NULL);
    RB_ASSERT_ARG(pred != NULL);

    if (x->left != NULL)
    {
        __rb_tree_find_maximum(x->left, pred);
        goto done;
    }

    struct rb_tree_node *y = x->parent;

    while (y != NULL && (x == y->left))
    {
        x = y;
        y = y->parent;
    }

    *pred = y;

done:
    return ret;
}