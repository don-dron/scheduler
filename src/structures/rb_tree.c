#include <structures/rb_tree.h>

#include <stdlib.h>
#include <string.h>

#define COLOR_BLACK 0x0ull
#define COLOR_RED 0x1ull

#define RB_TREE_NODE_GET_COLOR(_node) ((_node)->color)
#define RB_TREE_NODE_SET_COLOR(_node, _color) \
    do                                        \
    {                                         \
        ((_node)->color = (_color));          \
    } while (0)
#define RB_TREE_NODE_GET_PARENT(_node) ((_node)->parent)
#define RB_TREE_NODE_SET_PARENT(_node, _parent) \
    do                                          \
    {                                           \
        ((_node)->parent = (_parent));          \
    } while (0)

static int __rb_tree_cmp_mapper(void *state, const void *lhs, const void *rhs)
{
    rb_cmp_func_t cmp = state;
    return cmp(lhs, rhs);
}

rb_result_t rb_tree_new_ex(struct rb_tree *tree,
                           rb_cmp_func_ex_t compare,
                           void *state)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(compare != NULL);

    tree->root = NULL;
    tree->compare = compare;
    tree->state = state;
    tree->rightmost = NULL;

    return ret;
}

rb_result_t rb_tree_new(struct rb_tree *tree,
                        rb_cmp_func_t compare)
{
    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(compare != NULL);

    return rb_tree_new_ex(tree, __rb_tree_cmp_mapper, (void *)compare);
}

rb_result_t rb_tree_destroy(struct rb_tree *tree)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);

    memset(tree, 0, sizeof(struct rb_tree));

    return ret;
}

rb_result_t rb_tree_empty(struct rb_tree *tree,
                          int *is_empty)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(is_empty != NULL);

    *is_empty = !!(tree->root == NULL);

    return ret;
}

rb_result_t rb_tree_find(struct rb_tree *tree,
                         const void *key,
                         struct rb_tree_node **value)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(value != NULL);

    *value = NULL;

    if (RB_UNLIKELY(tree->root == NULL))
    {
        ret = RB_NOT_FOUND;
        goto done;
    }

    struct rb_tree_node *node = tree->root;

    while (node != NULL)
    {
        int compare = tree->compare(tree->state, key, node->key);

        if (compare < 0)
        {
            node = node->left;
        }
        else if (compare == 0)
        {
            break;
        }
        else
        {
            node = node->right;
        }
    }

    if (node == NULL)
    {
        ret = RB_NOT_FOUND;
        goto done;
    }

    *value = node;

done:
    return ret;
}

static inline struct rb_tree_node *__helper_get_grandparent(struct rb_tree_node *node)
{
    struct rb_tree_node *parent_node = RB_TREE_NODE_GET_PARENT(node);

    if (parent_node == NULL)
    {
        return NULL;
    }

    return RB_TREE_NODE_GET_PARENT(parent_node);
}

static inline void __helper_rotate_left(struct rb_tree *tree,
                                        struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;
    struct rb_tree_node *y = x->right;

    x->right = y->left;

    if (y->left != NULL)
    {
        struct rb_tree_node *yleft = y->left;
        RB_TREE_NODE_SET_PARENT(yleft, x);
    }

    RB_TREE_NODE_SET_PARENT(y, RB_TREE_NODE_GET_PARENT(x));

    if (RB_TREE_NODE_GET_PARENT(x) == NULL)
    {
        tree->root = y;
    }
    else
    {
        struct rb_tree_node *xp = RB_TREE_NODE_GET_PARENT(x);
        if (x == xp->left)
        {
            xp->left = y;
        }
        else
        {
            xp->right = y;
        }
    }

    y->left = x;
    RB_TREE_NODE_SET_PARENT(x, y);
}

static inline void __helper_rotate_right(struct rb_tree *tree,
                                         struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;
    struct rb_tree_node *y = x->left;

    x->left = y->right;

    if (y->right != NULL)
    {
        struct rb_tree_node *yright = y->right;
        RB_TREE_NODE_SET_PARENT(yright, x);
    }

    RB_TREE_NODE_SET_PARENT(y, RB_TREE_NODE_GET_PARENT(x));

    if (RB_TREE_NODE_GET_PARENT(x) == NULL)
    {
        tree->root = y;
    }
    else
    {
        struct rb_tree_node *xp = RB_TREE_NODE_GET_PARENT(x);
        if (x == xp->left)
        {
            xp->left = y;
        }
        else
        {
            xp->right = y;
        }
    }

    y->right = x;
    RB_TREE_NODE_SET_PARENT(x, y);
}

static void __helper_rb_tree_insert_rebalance(struct rb_tree *tree,
                                              struct rb_tree_node *node)
{
    struct rb_tree_node *new_node_parent = RB_TREE_NODE_GET_PARENT(node);

    if (new_node_parent != NULL && RB_TREE_NODE_GET_COLOR(new_node_parent) != COLOR_BLACK)
    {
        struct rb_tree_node *pnode = node;

        while ((tree->root != pnode) && (RB_TREE_NODE_GET_PARENT(pnode) != NULL) &&
               (RB_TREE_NODE_GET_COLOR(
                    RB_TREE_NODE_GET_PARENT(pnode)) == COLOR_RED))
        {
            struct rb_tree_node *parent = RB_TREE_NODE_GET_PARENT(pnode);
            struct rb_tree_node *grandparent = __helper_get_grandparent(pnode);
            struct rb_tree_node *uncle = NULL;
            int uncle_is_left;

            assert(RB_TREE_NODE_GET_COLOR(pnode) == COLOR_RED);

            if (parent == grandparent->left)
            {
                uncle_is_left = 0;
                uncle = grandparent->right;
            }
            else
            {
                uncle_is_left = 1;
                uncle = grandparent->left;
            }

            if (uncle && RB_TREE_NODE_GET_COLOR(uncle) == COLOR_RED)
            {
                RB_TREE_NODE_SET_COLOR(parent, COLOR_BLACK);
                RB_TREE_NODE_SET_COLOR(uncle, COLOR_BLACK);

                RB_TREE_NODE_SET_COLOR(grandparent, COLOR_RED);
                pnode = grandparent;
            }
            else
            {
                if (!uncle_is_left && parent->right == pnode)
                {
                    pnode = RB_TREE_NODE_GET_PARENT(pnode);
                    __helper_rotate_left(tree, pnode);
                }
                else if (uncle_is_left && parent->left == pnode)
                {
                    pnode = RB_TREE_NODE_GET_PARENT(pnode);
                    __helper_rotate_right(tree, pnode);
                }

                parent = RB_TREE_NODE_GET_PARENT(pnode);
                RB_TREE_NODE_SET_COLOR(parent, COLOR_BLACK);

                grandparent = __helper_get_grandparent(pnode);
                RB_TREE_NODE_SET_COLOR(grandparent, COLOR_RED);
                if (!uncle_is_left)
                {
                    __helper_rotate_right(tree, grandparent);
                }
                else
                {
                    __helper_rotate_left(tree, grandparent);
                }
            }
        }

        struct rb_tree_node *tree_root = tree->root;
        RB_TREE_NODE_SET_COLOR(tree_root, COLOR_BLACK);
    }
}

rb_result_t rb_tree_insert(struct rb_tree *tree,
                           const void *key,
                           struct rb_tree_node *node)
{
    rb_result_t ret = RB_OK;

    int rightmost = 1;
    struct rb_tree_node *nd = NULL;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(node != NULL);

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = key;

    if (RB_UNLIKELY(tree->root == NULL))
    {
        tree->root = node;
        tree->rightmost = node;
        RB_TREE_NODE_SET_COLOR(node, COLOR_BLACK);
        goto done;
    }

    nd = tree->root;
    RB_TREE_NODE_SET_COLOR(node, COLOR_RED);

    rightmost = 1;

    while (nd != NULL)
    {
        int compare = tree->compare(tree->state, node->key, nd->key);

        if (compare == 0)
        {
            ret = RB_DUPLICATE;
            goto done;
        }

        if (compare < 0)
        {
            rightmost = 0;
            if (nd->left == NULL)
            {
                nd->left = node;
                break;
            }
            else
            {
                nd = nd->left;
            }
        }
        else
        {
            if (nd->right == NULL)
            {
                nd->right = node;
                break;
            }
            else
            {
                nd = nd->right;
            }
        }
    }

    RB_TREE_NODE_SET_PARENT(node, nd);

    if (1 == rightmost)
    {
        tree->rightmost = node;
    }

    __helper_rb_tree_insert_rebalance(tree, node);

done:
    return ret;
}

rb_result_t rb_tree_find_or_insert(struct rb_tree *tree,
                                   void *key,
                                   struct rb_tree_node *new_candidate,
                                   struct rb_tree_node **value)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(value != NULL);
    RB_ASSERT_ARG(new_candidate != NULL);

    *value = NULL;
    new_candidate->key = key;

    struct rb_tree_node *node = tree->root;

    if (RB_UNLIKELY(tree->root == NULL))
    {
        tree->root = new_candidate;
        tree->rightmost = new_candidate;
        RB_TREE_NODE_SET_COLOR(new_candidate, COLOR_BLACK);
        node = new_candidate;
        goto done;
    }

    struct rb_tree_node *node_prev = NULL;
    int dir = 0, rightmost = 1;
    while (node != NULL)
    {
        int compare = tree->compare(tree->state, key, node->key);

        if (compare < 0)
        {
            node_prev = node;
            dir = 0;
            node = node->left;
            rightmost = 0;
        }
        else if (compare == 0)
        {
            break;
        }
        else
        {
            node_prev = node;
            dir = 1;
            node = node->right;
        }
    }

    if (node == NULL)
    {
        if (dir == 0)
        {
            rightmost = 0;
            node_prev->left = new_candidate;
        }
        else
        {
            node_prev->right = new_candidate;
        }

        RB_TREE_NODE_SET_PARENT(new_candidate, node_prev);

        node = new_candidate;
        RB_TREE_NODE_SET_COLOR(node, COLOR_RED);

        if (1 == rightmost)
        {
            tree->rightmost = new_candidate;
        }

        __helper_rb_tree_insert_rebalance(tree, node);
    }

done:
    *value = node;

    return ret;
}

static struct rb_tree_node *__helper_rb_tree_find_minimum(struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;

    while (x->left != NULL)
    {
        x = x->left;
    }

    return x;
}

static struct rb_tree_node *__helper_rb_tree_find_maximum(struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;

    while (x->right != NULL)
    {
        x = x->right;
    }

    return x;
}

static struct rb_tree_node *__helper_rb_tree_find_successor(struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;

    if (x->right != NULL)
    {
        return __helper_rb_tree_find_minimum(x->right);
    }

    struct rb_tree_node *y = RB_TREE_NODE_GET_PARENT(x);

    while (y != NULL && x == y->right)
    {
        x = y;
        y = RB_TREE_NODE_GET_PARENT(y);
    }

    return y;
}

static struct rb_tree_node *__helper_rb_tree_find_predecessor(struct rb_tree_node *node)
{
    struct rb_tree_node *x = node;

    if (x->left != NULL)
    {
        return __helper_rb_tree_find_maximum(x->left);
    }

    struct rb_tree_node *y = RB_TREE_NODE_GET_PARENT(x);

    while (y != NULL && x == y->left)
    {
        x = y;
        y = RB_TREE_NODE_GET_PARENT(y);
    }

    return y;
}

static void __helper_rb_tree_swap_node(struct rb_tree *tree,
                                       struct rb_tree_node *x,
                                       struct rb_tree_node *y)
{
    struct rb_tree_node *left = x->left;
    struct rb_tree_node *right = x->right;
    struct rb_tree_node *parent = RB_TREE_NODE_GET_PARENT(x);

    RB_TREE_NODE_SET_PARENT(y, parent);

    if (parent != NULL)
    {
        if (parent->left == x)
        {
            parent->left = y;
        }
        else
        {
            parent->right = y;
        }
    }
    else
    {
        if (tree->root == x)
        {
            tree->root = y;
        }
    }

    y->right = right;
    if (right != NULL)
    {
        RB_TREE_NODE_SET_PARENT(right, y);
    }
    x->right = NULL;

    y->left = left;
    if (left != NULL)
    {
        RB_TREE_NODE_SET_PARENT(left, y);
    }
    x->left = NULL;

    RB_TREE_NODE_SET_COLOR(y, RB_TREE_NODE_GET_COLOR(x));
    x->parent = NULL;
}

static void __helper_rb_tree_delete_rebalance(struct rb_tree *tree,
                                              struct rb_tree_node *node,
                                              struct rb_tree_node *parent,
                                              int node_is_left)
{
    struct rb_tree_node *x = node;
    struct rb_tree_node *xp = parent;
    int is_left = node_is_left;

    while (x != tree->root && (x == NULL || RB_TREE_NODE_GET_COLOR(x) == COLOR_BLACK))
    {
        struct rb_tree_node *w = is_left ? xp->right : xp->left;

        if (w != NULL && RB_TREE_NODE_GET_COLOR(w) == COLOR_RED)
        {
            RB_TREE_NODE_SET_COLOR(w, COLOR_BLACK);
            RB_TREE_NODE_SET_COLOR(xp, COLOR_RED);
            if (is_left)
            {
                __helper_rotate_left(tree, xp);
            }
            else
            {
                __helper_rotate_right(tree, xp);
            }
            w = is_left ? xp->right : xp->left;
        }

        struct rb_tree_node *wleft = w != NULL ? w->left : NULL;
        struct rb_tree_node *wright = w != NULL ? w->right : NULL;
        if ((wleft == NULL || RB_TREE_NODE_GET_COLOR(wleft) == COLOR_BLACK) &&
            (wright == NULL || RB_TREE_NODE_GET_COLOR(wright) == COLOR_BLACK))
        {
            if (w != NULL)
            {
                RB_TREE_NODE_SET_COLOR(w, COLOR_RED);
            }
            x = xp;
            xp = RB_TREE_NODE_GET_PARENT(x);
            is_left = xp && (x == xp->left);
        }
        else
        {
            if (is_left && (wright == NULL || RB_TREE_NODE_GET_COLOR(wright) == COLOR_BLACK))
            {
                RB_TREE_NODE_SET_COLOR(w, COLOR_RED);
                if (wleft)
                {
                    RB_TREE_NODE_SET_COLOR(wleft, COLOR_BLACK);
                }
                __helper_rotate_right(tree, w);
                w = xp->right;
            }
            else if (!is_left && (wleft == NULL || RB_TREE_NODE_GET_COLOR(wleft) == COLOR_BLACK))
            {
                RB_TREE_NODE_SET_COLOR(w, COLOR_RED);
                if (wright)
                {
                    RB_TREE_NODE_SET_COLOR(wright, COLOR_BLACK);
                }
                __helper_rotate_left(tree, w);
                w = xp->left;
            }

            wleft = w->left;
            wright = w->right;

            RB_TREE_NODE_SET_COLOR(w, RB_TREE_NODE_GET_COLOR(xp));
            RB_TREE_NODE_SET_COLOR(xp, COLOR_BLACK);

            if (is_left && wright != NULL)
            {
                RB_TREE_NODE_SET_COLOR(wright, COLOR_BLACK);
                __helper_rotate_left(tree, xp);
            }
            else if (!is_left && wleft != NULL)
            {
                RB_TREE_NODE_SET_COLOR(wleft, COLOR_BLACK);
                __helper_rotate_right(tree, xp);
            }
            x = tree->root;
        }
    }

    if (x != NULL)
    {
        RB_TREE_NODE_SET_COLOR(x, COLOR_BLACK);
    }
}

rb_result_t rb_tree_remove(struct rb_tree *tree,
                           struct rb_tree_node *node)
{
    rb_result_t ret = RB_OK;

    RB_ASSERT_ARG(tree != NULL);
    RB_ASSERT_ARG(node != NULL);

    struct rb_tree_node *y;

    if (node->left == NULL || node->right == NULL)
    {
        y = node;
        if (node == tree->rightmost)
        {
            tree->rightmost = __helper_rb_tree_find_predecessor(node);
        }
    }
    else
    {
        y = __helper_rb_tree_find_successor(node);
    }

    struct rb_tree_node *x, *xp;

    if (y->left != NULL)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }

    if (x != NULL)
    {
        RB_TREE_NODE_SET_PARENT(x, RB_TREE_NODE_GET_PARENT(y));
        xp = RB_TREE_NODE_GET_PARENT(x);
    }
    else
    {
        xp = RB_TREE_NODE_GET_PARENT(y);
    }

    int is_left = 0;
    if (RB_TREE_NODE_GET_PARENT(y) == NULL)
    {
        tree->root = x;
        xp = NULL;
    }
    else
    {
        struct rb_tree_node *yp = RB_TREE_NODE_GET_PARENT(y);
        if (y == yp->left)
        {
            yp->left = x;
            is_left = 1;
        }
        else
        {
            yp->right = x;
            is_left = 0;
        }
    }

    int y_color = RB_TREE_NODE_GET_COLOR(y);

    if (y != node)
    {
        __helper_rb_tree_swap_node(tree, node, y);
        if (xp == node)
        {
            xp = y;
        }
    }

    if (y_color == COLOR_BLACK)
    {
        __helper_rb_tree_delete_rebalance(tree, x, xp, is_left);
    }

    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    return ret;
}