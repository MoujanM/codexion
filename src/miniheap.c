
#include "codex.h"

// mini-heap implementation

// if keeping this, move to utils
// add similar child ops
int     get_parent(int  idx)
{
    return ((idx - 1) / 2);
}
void    swap(t_request *x, t_request *y)
{
    t_request   tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;

}

void    bubble_up(t_heap *heap, int idx)
{
    while (idx > 0 && heap->request[idx].priority <
        heap->request[get_parent(idx)].priority)
    {
            swap(&heap->request[idx], &heap->request[get_parent(idx)]);
            idx = get_parent(idx);
    }
}

void    bubble_down(t_heap *heap, int idx)
{
    int     left_child;
    int     right_child;
    int     smallest;

    while (true)
    {
        smallest = idx;
        left_child = 2 * idx + 1;
        right_child = 2 * idx + 2;
        if (left_child < heap->size && heap->request[left_child].priority <
        heap->request[smallest].priority)
            smallest = left_child;
        if (right_child < heap->size && heap->request[right_child].priority <
        heap->request[smallest].priority)
            smallest = right_child;
        if (smallest == idx)
            break ;
        swap(&heap->request[smallest], &heap->request[idx]);
        idx = smallest;
    }
}

void    insert(t_heap *heap, t_request request)
{
    int     i;

    heap->request[heap->size] = request;
    i = heap->size;
    heap->size++;
    bubble_up(heap, i);
}

void   pop_min(t_heap *heap)
{
    t_request   root;

    root = heap->request[0];
    heap->size--;
    heap->request[0] = heap->request[heap->size];
    bubble_down(heap, 0);
    // return (root); check if return actually needed
}

