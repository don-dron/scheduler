#include <structures/fibonacci_heap.h>
#include <math.h>

struct fibonacci_heap_node *fibonacci_heap_node_alloc()
{
    struct fibonacci_heap_node *node = (struct fibonacci_heap_node *)malloc(sizeof(struct fibonacci_heap_node));
    node->if_lost_child = 0;
    node->degree = 0;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    node->child = NULL;
    node->key = NULL;
    return node;
}

static void fibonacci_heap_node_list_insert(struct fibonacci_heap_node *list_node, struct fibonacci_heap_node *node)
{
    node->right = list_node->right;
    list_node->right = node;
    node->left = list_node;
    node->right->left = node;
}

static struct fibonacci_heap_node *fibonacci_heap_node_list_remove(struct fibonacci_heap_node *node)
{
    if (node->left == node)
        return NULL;
    struct fibonacci_heap_node *right = node->right;
    struct fibonacci_heap_node *left = node->left;
    left->right = right;
    right->left = left;
    return right;
}

static void fibonacci_heap_node_list_link_two_list(struct fibonacci_heap_node *node1, struct fibonacci_heap_node *node2)
{
    if (node1 == NULL || node2 == NULL)
        return;
    struct fibonacci_heap_node *node1_left = node1->left;
    struct fibonacci_heap_node *node2_left = node2->left;
    node1->left = node2_left;
    node2_left->right = node1;
    node2->left = node1_left;
    node1_left->right = node2;
}

struct fibonacci_heap *fibonacci_heap_alloc(fibonacci_key_cmp_t key_cmp, fibonacci_key_min_t key_min, fibonacci_key_pr_t key_pr)
{
    struct fibonacci_heap *fh = (struct fibonacci_heap *)malloc(sizeof(struct fibonacci_heap));
    fh->n = 0;
    fh->min = 0;
    fh->key_cmp = key_cmp;
    fh->key_min = key_min;
    fh->key_pr = key_pr;

    return fh;
}

static void fibonacci_heapfreenode_list(struct fibonacci_heap_node *node)
{
    if (node == NULL)
        return;
    struct fibonacci_heap_node *_node = node->right;
    while (_node != node)
    {
        if (node->degree > 0)
            fibonacci_heapfreenode_list(node->child);
        _node = _node->right;
        free(_node->left);
    }
    if (node->degree > 0)
        fibonacci_heapfreenode_list(node->child);
    free(node);
}

void fibonacci_heap_free(struct fibonacci_heap *fh)
{
    if (fh == NULL)
        return;
    fibonacci_heapfreenode_list(fh->min);
    free(fh);
}

struct fibonacci_heap *fibonacci_heap_union(struct fibonacci_heap *fh_1, struct fibonacci_heap *fh_2)
{
    struct fibonacci_heap *fh = NULL;
    if (fh_1 == NULL || fh_2 == NULL)
    {
        if (fh_1 == NULL)
            fh_1 = fh_2;
        if (fh_1 != NULL)
        {
            fh = fibonacci_heap_alloc(fh_1->key_cmp, fh_1->key_min, fh_1->key_pr);
            fh->n = fh_1->n;
            fh->min = fh_1->min;
        }
        free(fh_1);
        free(fh_2);
        return fh;
    }

    if (fh_1->n == 0 || fh_2->n == 0)
    {
        fh = fibonacci_heap_alloc(fh_1->key_cmp, fh_1->key_min, fh_1->key_pr);
        if (fh_1->n == 0)
        {
            free(fh_1);
            fh_1 = fh_2;
        }
        if (fh_1->n != 0)
        {
            fh->n = fh_1->n;
            fh->min = fh_1->min;
        }
        free(fh_1);
        free(fh_2);
        return fh;
    }

    fh->n = fh_1->n + fh_2->n;
    fh->min = fh_1->min;
    if (fh->key_cmp(fh_1->min, fh_2->min) > 0)
        fh->min = fh_2->min;

    struct fibonacci_heap_node *node1 = fh_1->min;
    struct fibonacci_heap_node *node2 = fh_2->min;
    fibonacci_heap_node_list_link_two_list(node1, node2);
    free(fh_1);
    free(fh_2);
    return fh;
}

void fibonacci_heap_insert_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node)
{
    if (fh == NULL || node == NULL)
        return;
    node->parent = NULL;
    node->child = NULL;
    node->degree = 0;
    if (fh->n == 0)
    {
        fh->n = 1;
        fh->min = node;
        node->left = node;
        node->right = node;
        return;
    }
    fibonacci_heap_node_list_insert(fh->min, node);
    if (fh->key_cmp(node->key, fh->min->key) < 0)
        fh->min = node;
    fh->n += 1;
}

struct fibonacci_heap_node *fibonacci_heap_get_min_node(struct fibonacci_heap *fh)
{
    if (fh == NULL)
        return NULL;
    return fh->min;
}

static void fibonacci_heap_extract_min_node_fix(struct fibonacci_heap *fh)
{
    if (fh == NULL || fh->n == 0)
        return;

    size_t D = 0;

    while (((size_t)(1 << D)) > ((size_t)fh->n))
    {
        D++;
    }
    D++;

    struct fibonacci_heap_node **A = (struct fibonacci_heap_node **)malloc(sizeof(struct fibonacci_heap_node) * (size_t)D);
    for (size_t i = 0; i < D; ++i)
        A[i] = NULL;

    struct fibonacci_heap_node *start_node = fh->min, *node = NULL;
    node = start_node;
    do
    {
        struct fibonacci_heap_node *x = node;
        struct fibonacci_heap_node *next_node = node->right;
        size_t d = node->degree;
        while (A[d] != NULL)
        {
            struct fibonacci_heap_node *y = A[d];
            if (fh->key_cmp(x->key, y->key) > 0)
            {
                struct fibonacci_heap_node *t = x;
                x = y;
                y = t;
            }
            do
            {
                if (next_node == start_node)
                    break;
                if (y == next_node)
                {
                    next_node = next_node->right;
                    break;
                }
                if (y == start_node)
                {
                    start_node = start_node->right;
                    break;
                }
            } while (0);
            {
                fibonacci_heap_node_list_remove(y);

                y->parent = x;
                if (x->child == NULL)
                {
                    x->child = y;
                    y->left = y;
                    y->right = y;
                }
                else
                    fibonacci_heap_node_list_insert(x->child, y);
                x->degree += 1;
                y->if_lost_child = 0;
            }
            A[d] = NULL;
            d += 1;
        }
        A[d] = x;
        node = next_node;
        next_node = next_node->right;
    } while (node != start_node);
    fh->min = NULL;
    for (size_t i = 0; i < D; ++i)
    {
        if (A[i] != NULL)
        {
            if (fh->min == NULL)
            {
                fh->min = A[i];
                fh->min->left = fh->min;
                fh->min->right = fh->min;
            }
            else
            {
                fibonacci_heap_node_list_insert(fh->min, A[i]);
                if (fh->key_cmp(A[i]->key, fh->min->key) < 0)
                    fh->min = A[i];
            }
        }
    }
}

struct fibonacci_heap_node *fibonacci_heap_extract_min_node(struct fibonacci_heap *fh)
{
    if (fh == NULL)
        return NULL;
    struct fibonacci_heap_node *min_node = fh->min;
    if (min_node == NULL)
        return NULL;
    if (min_node->degree > 0)
    {
        struct fibonacci_heap_node *child = min_node->child;
        struct fibonacci_heap_node *next_child = child->right;
        while (next_child != child)
        {
            next_child->parent = NULL;
            next_child = next_child->right;
        }
        child->parent = NULL;
        fibonacci_heap_node_list_link_two_list(min_node, child);
    }
    struct fibonacci_heap_node *root = fibonacci_heap_node_list_remove(min_node);
    if (min_node->right == min_node)
    {
        fh->min = NULL;
    }
    else
    {
        fh->min = root;
        fibonacci_heap_extract_min_node_fix(fh);
    }
    fh->n -= 1;
    return min_node;
}

static void fibonacci_heap_decrease_key_cut(struct fibonacci_heap *fh, struct fibonacci_heap_node *child, struct fibonacci_heap_node *parent)
{
    parent->child = fibonacci_heap_node_list_remove(child);
    parent->degree -= 1;
    if (parent->degree == 0)
        parent->child = NULL;

    fibonacci_heap_node_list_insert(fh->min, child);
    child->parent = NULL;
    child->if_lost_child = 0;
}

static void fibonacci_heap_decrease_key_cascading_cut(struct fibonacci_heap *fh, struct fibonacci_heap_node *node)
{
    struct fibonacci_heap_node *parent = node->parent;
    if (parent != NULL)
    {
        if (node->if_lost_child == 0)
        {
            node->if_lost_child = 1;
        }
        else
        {
            fibonacci_heap_decrease_key_cut(fh, node, parent);
            fibonacci_heap_decrease_key_cascading_cut(fh, parent);
        }
    }
}

void *fibonacci_heap_decrease_key_by_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node, void *key)
{
    if (fh == NULL || node == NULL || key == NULL)
        return NULL;

    if (fh->key_cmp(key, node->key) >= 0)
        return NULL;

    void *old_key = node->key;
    node->key = key;

    struct fibonacci_heap_node *parent = node->parent;
    if (parent != NULL && fh->key_cmp(node->key, parent->key) < 0)
    {
        fibonacci_heap_decrease_key_cut(fh, node, parent);
        fibonacci_heap_decrease_key_cascading_cut(fh, parent);
    }

    if (fh->key_cmp(node->key, fh->min->key))
        fh->min = node;

    return old_key;
}

void fibonacci_heap_delete_node(struct fibonacci_heap *fh, struct fibonacci_heap_node *node)
{
    if (fh == NULL || node == NULL)
        return;
    fibonacci_heap_decrease_key_by_node(fh, node, fh->key_min());
    fibonacci_heap_extract_min_node(fh);
}

static struct fibonacci_heap_node *fibonacci_heap_node_list_search(
    struct fibonacci_heap *fh,
    struct fibonacci_heap_node *node,
    void *key);

static struct fibonacci_heap_node *fibonacci_heap_node_list_search(
    struct fibonacci_heap *fh,
    struct fibonacci_heap_node *node,
    void *key)
{
    if (node == NULL)
        return NULL;
    struct fibonacci_heap_node *_node = node->right;

    while (_node != node)
    {
        if (fh->key_cmp(_node->key, key) == 0)
            return _node;

        if (_node->degree > 0 && fh->key_cmp(key, _node->key) > 0)
        {
            struct fibonacci_heap_node *ret = fibonacci_heap_node_list_search(fh, _node->child, key);
            if (ret != NULL)
                return ret;
        }
        _node = _node->right;
    }

    if (fh->key_cmp(_node->key, key) == 0)
        return _node;

    if (_node->degree > 0 && fh->key_cmp(key, _node->key))
    {
        struct fibonacci_heap_node *ret = fibonacci_heap_node_list_search(fh, _node->child, key);
        if (ret != NULL)
            return ret;
    }
    return NULL;
}

struct fibonacci_heap_node *fibonacci_heap_search_key(struct fibonacci_heap *fh, void *key)
{
    if (fh == NULL || key == NULL)
        return NULL;
    return fibonacci_heap_node_list_search(fh, fh->min, key);
}

static void fibonacci_heap_node_pr(struct fibonacci_heap *fh, struct fibonacci_heap_node *node, int deep)
{
    if (node == NULL)
        return;

    int _deep = deep;
    while (_deep)
    {
        printf("|  ");
        _deep -= 1;
    }

    printf("[node %lu ] p = %lu , l = %lu, r = %lu, c = %lu, d = %lu, m = %d, KEY = ",
           (size_t)node, (size_t)node->parent, (size_t)node->left, (size_t)node->right, (size_t)node->child, node->degree, node->if_lost_child);
    fh->key_pr(node->key);
    printf("\n");

    if (node->child != NULL)
    {
        struct fibonacci_heap_node *_node = node->child;
        do
        {
            fibonacci_heap_node_pr(fh, _node, deep + 1);
            _node = _node->right;
        } while (_node != node->child);
    }
}

void fibonacci_heap_pr(struct fibonacci_heap *fh)
{
    printf("\n");
    if (fh == NULL)
    {
        printf("heap NULL\n\n");
        return;
    }
    if (fh->n == 0)
    {
        printf("heap empty\n\n");
        return;
    }
    printf("[HEAP] n = %lu , min = %lu\n", fh->n, (size_t)fh->min);
    struct fibonacci_heap_node *node = fh->min;
    do
    {
        fibonacci_heap_node_pr(fh, node, 0);
        node = node->right;
    } while (node != fh->min);
    printf("\n");
}