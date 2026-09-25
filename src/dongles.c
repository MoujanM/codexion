/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:49 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 18:46:23 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static void	pick_up(t_dongle *first, t_dongle *second)
{
	lock_pair(first, second);
	pop_min(&first->request_heap);
	pop_min(&second->request_heap);
	first->is_available = false;
	second->is_available = false;
	unlock_pair(first, second);
}

void	wait_on_dongle(t_dongle *dongle, t_coder *coder)
{
	long				t_cooldown;
	struct timespec		t_deadline;

	pthread_mutex_lock(&dongle->d_lock);
	while (!is_stopped(coder->sim))
	{
		if (dongle->is_available && (dongle->last_release == 0
				|| get_time_ms() >= dongle->last_release + dongle->cooldown)
			&& dongle->request_heap.request[0].coder_id == coder->coder_id)
			break ;
		t_cooldown = dongle->last_release + dongle->cooldown;
		if (dongle->last_release == 0 || get_time_ms() >= t_cooldown)
			pthread_cond_wait(&dongle->d_cond, &dongle->d_lock);
		else
		{
			t_deadline.tv_sec = t_cooldown / 1000;
			t_deadline.tv_nsec = (t_cooldown % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->d_cond, &dongle->d_lock,
				&t_deadline);
		}
	}
	pthread_mutex_unlock(&dongle->d_lock);
}

int	take_if_available(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (is_stopped(coder->sim))
		return (1);
	if (is_ready(first, coder) && is_ready(second, coder))
	{
		pick_up(first, second);
		return (0);
	}
	else
	{
		if (!is_ready(first, coder))
			wait_on_dongle(first, coder);
		else
			wait_on_dongle(second, coder);
	}
	return (1);
}

bool	is_ready(t_dongle *dongle, t_coder *coder)
{
	bool	cooled;
	bool	mine;
	bool	available;

	pthread_mutex_lock(&dongle->d_lock);
	cooled = (dongle->last_release == 0
			|| get_time_ms() >= dongle->last_release + dongle->cooldown);
	mine = dongle->request_heap.request[0].coder_id == coder->coder_id;
	available = dongle->is_available;
	pthread_mutex_unlock(&dongle->d_lock);
	return (available && cooled && mine);
}

int	grab_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (is_stopped(coder->sim))
		return (1);
	first = coder->dongle_1;
	second = coder->dongle_2;
	register_requests(coder, first, second);
	while (!is_stopped(coder->sim))
	{
		if (take_if_available(coder, first, second) == 0)
		{
			print_log(coder, elapsed_time_ms(coder->sim), GRAB);
			break ;
		}
	}
	return (0);
}
