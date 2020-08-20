#include <structures/thin_heap.h>

int is_thin_node(thin_node *node)
{
    if (node->rank == 1)
    {
        return node->child == 0;
    }
    else
    {
        return node->child->rank + 1 != node->rank;
    }
}

thin_heap *create_thin_heap(THIN_HEAP_COMPARATOR comparator)
{
    thin_heap *heap = (thin_heap *)malloc(sizeof(thin_heap));
    heap->first = 0;
    heap->last = 0;
    heap->comparator = comparator;
    return heap;
}

void insert_to_thin_heap(thin_heap *heap, thin_node *node)
{
    if (heap->first == 0)
    {
        heap->first = node;
        heap->last = node;
    }
    else
    {
        if (heap->comparator(node, heap->first))
        {
            node->right = heap->first;
            heap->first = node;
        }
        else
        {
            heap->last->right = node;
            heap->last = node;
        }
    }
}

thin_node *get_thin_heap_min(thin_heap *heap)
{
    return heap->first;
}

thin_heap *merge_thin_heaps(thin_heap *h1, thin_heap *h2)
{
    if (h1->comparator != h2->comparator)
    {
        abort();
    }

    if (h1->first == 0)
    {
        return h2;
    }
    else
    {
        if (h2->first == 0)
        {
            return h1;
        }
        else
        {
            if (h1->comparator(h1->first, h2->first))
            {
                h1->last->right = h2->first;
                h1->last = h2->last;
                return h1;
            }
            else
            {
                h2->last->right = h1->first;
                h2->last = h1->last;
                return h2;
            }
        }
    }
}

thin_node *extract_min_thin_heap(thin_heap *heap)
{
    thin_node *result = heap->first;
    heap->first = heap->first->right;
    if (heap->first == 0)
    {
        heap->last = 0;
    }

    thin_node *node = result->child;
    thin_node *next;
    while (node != 0)
    {
        if (is_thin_node(node))
        {
            node->rank = node->rank - 1;
        }
        node->left = 0;
        next = node->right;
        insert_to_thin_heap(heap, node);
        node = next;
    }

    int max = -1;
    node = heap->first;

    size_t n = 50;
    thin_node **aux = (thin_node **)calloc(sizeof(thin_node *), n);

    for (size_t i = 0; i < n; i++)
    {
        aux[n] = 0;
    }

    while (node != 0)
    {
        while (aux[node->rank] != 0)
        {
            next = node->right;
            if (heap->comparator(aux[node->rank], node))
            {   
                thin_node* temp = aux[node->rank];
                aux[node->rank] = node;
                node = temp;
            }
            aux[node->rank]->right = node->child;
            node->child->left = aux[node->rank];
            aux[node->rank]->left = node;
            node->child = aux[node->rank];
            aux[node->rank] = 0;
            node->rank = node->rank + 1;
        }
        aux[node->rank] = node;
        if (node->rank > max)
        {
            max = node->rank;
        }
        node = next;
    }

    heap = create_thin_heap(heap->comparator);
    int i = 0;
    while (i <= max)
    {
        insert_to_thin_heap(heap, aux[i]);
        i = i + 1;
    }
    return result;
}

void decrease_key_thin_heap(thin_heap *heap, thin_node *node, DECREASE_KEY_FUNCTION func)
{
    func(node);

    if (!node->left)
    {
        if (heap->comparator(node, heap->first))
        {
            heap->first->right = heap->last;
            heap->last = node->right;
            node->right = 0;
            heap->first = node;
        }
        return;
    }

    thin_node *y = node->left;

    if (y->child == node)
    {

        y->child = node->right;
        if (node->right)
        {
            node->right->left = y;
        }
    }
    else
    {

        y->right = node->right;
        if (node->right)
        {
            node->right->left = y;
        }
    }

    node->right = node->left = 0;

    if (is_thin_node(node))
    {
        node->rank -= 1;
    }
    insert_to_thin_heap(heap, node);

    thin_node *cur;
    thin_node *z;
    int suc = 0;
    while (suc != 2)
    {
        suc = 0;

        if ((y->rank == 1 && !y->right) || (y->rank - y->right->rank >= 2))
        {
            if (!is_thin_node(y))
            {
                cur = y->child;
                y->child = y->child->right;

                if (y->right)
                {
                    y->child->left = y;
                }

                cur->left = y;
                cur->right = y->right;

                if (y->right)
                {
                    y->right->left = cur;
                }

                y->right = cur;
                return;
            }
            else
            {
                y->rank--;
                y = y->left;
            }
        }
        else
        {
            suc++;
        }

        if ((y->rank == 2 && !y->child))
        {
            y->rank -= 2;
        }
        else
        {
            if (y->child)
            {
                if (((y->rank - y->child->rank) == 3) || (!y->left))
                {
                    if (!y->left)
                    {
                        y->rank -= 1;
                        return;
                    }
                    z = y->left;
                    if (z->child == y)
                    {
                        z->child = y->right;
                        y->right->left = z;
                    }
                    else
                    {
                        z->right = y->right;
                        y->right->left = z;
                    }
                    y->right = y->left = 0;
                    insert_to_thin_heap(heap, y);
                    y = z;
                }
                else
                    suc++;
            }
            else
                suc++;
        }
    }
}

void delete_thin_heap(thin_heap *heap, thin_node *node, DELETE_DECREASE_KEY_FUNCTION func)
{
    decrease_key_thin_heap(heap, node, func);
    extract_min_thin_heap(heap);
}