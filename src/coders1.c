/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:49 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/04 17:54:53 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

t_request	make_request(t_coder *coder)
{
	t_request	request;
	int			burn_time;

	burn_time = coder->sim->args->time_to_burnout;
	request.coder_id = coder->coder_id;
	if (strcmp(coder->sim->args->scheduler, "fifo") == 0)
		request.priority = elapsed_time_ms(coder->sim);
	else if (strcmp(coder->sim->args->scheduler, "edf") == 0)
		request.priority = burn_time + coder->last_compile;
	return (request);
}

int	request_or_grab(t_coder *coder, t_dongle *dongle)
{
	t_request			request;
	long				to_cooldown;
	struct timespec		t;

	if (coder->sim->stop_sim)
		return (1);
	request = make_request(coder);
	pthread_mutex_lock(&dongle->d_lock);
	insert(&dongle->request_heap, request);
	while (!coder->sim->stop_sim && !(dongle->is_available
			&& dongle->request_heap.request[0].coder_id == coder->coder_id))
	{
		if (dongle->last_release == 0)
			continue;
		else
		{
			to_cooldown = dongle->last_release + dongle->cooldown;
			t.tv_sec = to_cooldown / 1000;
			t.tv_nsec = (to_cooldown % 1000) * 1000000;
			pthread_cond_timedwait(&dongle->d_cond, &dongle->d_lock, &t);
		}
	}
	pop_min(&dongle->request_heap);
	dongle->is_available = false;
	pthread_mutex_unlock(&dongle->d_lock);
	return (0);
}

int	grab_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->coder_id % 2 != 0)
	{
		first = coder->dongle_1;
		second = coder->dongle_2;
	}
	else
	{
		first = coder->dongle_2;
		second = coder->dongle_1;
	}
	if (request_or_grab(coder, first) != 0)
		return (1);
	print_log(coder, elapsed_time_ms(coder->sim), GRAB);
	if (request_or_grab(coder, second) != 0)
	{
		release_dongle(first, get_time_ms());
		return (1);
	}
	print_log(coder, elapsed_time_ms(coder->sim), GRAB);
	return (0);
}

void	release_dongle(t_dongle *dongle, long timestamp)
{
	pthread_mutex_lock(&dongle->d_lock);
	dongle->last_release = timestamp;
	dongle->is_available = true;
	pthread_cond_broadcast(&dongle->d_cond);
	pthread_mutex_unlock(&dongle->d_lock);
}
