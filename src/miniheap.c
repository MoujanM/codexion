/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniheap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:21 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 18:46:29 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	swap(t_req *x, t_req *y)
{
	t_req	tmp;

	tmp = *x;
	*x = *y;
	*y = tmp;
}

void	bubble_up(t_heap *heap, int idx)
{
	while (idx > 0 && has_priority(heap->request[idx],
			heap->request[get_parent(idx)]))
	{
		swap(&heap->request[idx], &heap->request[get_parent(idx)]);
		idx = get_parent(idx);
	}
}

void	bubble_down(t_heap *heap, int idx)
{
	int	left_child;
	int	right_child;
	int	smallest;

	while (true)
	{
		smallest = idx;
		left_child = 2 * idx + 1;
		right_child = 2 * idx + 2;
		if (left_child < heap->size && has_priority(
				heap->request[left_child], heap->request[smallest]))
			smallest = left_child;
		if (right_child < heap->size && has_priority(heap->request[right_child],
				heap->request[smallest]))
			smallest = right_child;
		if (smallest == idx)
			break ;
		swap(&heap->request[smallest], &heap->request[idx]);
		idx = smallest;
	}
}

void	insert(t_heap *heap, t_req request)
{
	int	i;

	heap->request[heap->size] = request;
	i = heap->size;
	heap->size++;
	bubble_up(heap, i);
}

void	pop_min(t_heap *heap)
{
	heap->size--;
	heap->request[0] = heap->request[heap->size];
	bubble_down(heap, 0);
}
