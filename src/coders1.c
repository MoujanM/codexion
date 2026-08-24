/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:49 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/07 13:12:19 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

t_req	make_request(t_coder *coder)
{
	t_req	request;
	char	*scheduler;
	int		burn_time;

	pthread_mutex_lock(&coder->sim->sim_lock);
	burn_time = coder->sim->args->time_to_burnout;
	scheduler = coder->sim->args->scheduler;
	pthread_mutex_unlock(&coder->sim->sim_lock);
	pthread_mutex_lock(&coder->lock);
	request.coder_id = coder->coder_id;
	request.sequence = next_sequence(coder->sim);
	if (strcmp(scheduler, "fifo") == 0)
		request.priority = elapsed_time_ms(coder->sim);
	else if (strcmp(scheduler, "edf") == 0)
		request.priority = burn_time + coder->last_compile;
	pthread_mutex_unlock(&coder->lock);
	return (request);
}

// void	manage_wait(t_coder *coder, t_dongle *dongle, int deadline)
// {
// 	long				to_cooldown;
// 	struct timespec		t;

// 	while (!is_stopped(coder->sim))
// 	{
// 		if (!dongle->is_available || dongle->request_heap.request[0].coder_id != coder->coder_id)
// 		{
// 			pthread_cond_wait(&dongle->d_cond, &dongle->d_lock);
// 			continue ;
// 		}
// 		to_cooldown = dongle->last_release + dongle->cooldown;
// 		if (dongle->last_release != 0 && get_time_ms() < to_cooldown)
// 		{
// 			t.tv_sec = to_cooldown / 1000;
// 			t.tv_nsec = (to_cooldown % 1000) * 1000000;
// 			pthread_cond_timedwait(&dongle->d_cond, &dongle->d_lock, &t);
// 			continue ;
// 		}
// 		break ;
// 	}
// }

// void	wait_for_dongle(t_dongle *dongle)
// {
// 	long				to_cooldown;
// 	struct timespec		t;

// 	if (dongle->last_release == 0)
// 		pthread_cond_wait(&dongle->d_cond, &dongle->d_lock);
// 	else
// 	{
// 		to_cooldown = dongle->last_release + dongle->cooldown;
// 		t.tv_sec = to_cooldown / 1000;
// 		t.tv_nsec = (to_cooldown % 1000) * 1000000;
// 		pthread_cond_timedwait(&dongle->d_cond, &dongle->d_lock, &t);
// 	}
// }

// int	request_or_grab(t_coder *coder, t_dongle *dongle)
// {
// 	t_req	request;
// 	int		max_wait;

// 	if (is_stopped(coder->sim))
// 		return (1);
// 	request = make_request(coder);
// 	pthread_mutex_lock(&dongle->d_lock);
// 	max_wait = dongle->cooldown / 2;
// 	insert(&dongle->request_heap, request);
// 	manage_wait(coder, dongle, max_wait);
// 	if (is_stopped(coder->sim))
// 	{
// 		pthread_mutex_unlock(&dongle->d_lock);
// 		return (1);
// 	}
// 	pop_min(&dongle->request_heap);
// 	dongle->is_available = false;
// 	pthread_mutex_unlock(&dongle->d_lock);
// 	return (0);
// }

// int	grab_dongles(t_coder *coder)
// {
// 	t_dongle	*first;
// 	t_dongle	*second;

// 	if (coder->coder_id % 2 != 0)
// 	{
// 		first = coder->dongle_1;
// 		second = coder->dongle_2;
// 	}
// 	else
// 	{
// 		first = coder->dongle_2;
// 		second = coder->dongle_1;
// 	}
// 	if (request_or_grab(coder, first) != 0)
// 		return (1);
// 	print_log(coder, elapsed_time_ms(coder->sim), GRAB);
// 	if (request_or_grab(coder, second) != 0)
// 	{
// 		release_dongle(first, get_time_ms());
// 		return (1);
// 	}
// 	print_log(coder, elapsed_time_ms(coder->sim), GRAB);
// 	return (0);
// }

void	release_dongle(t_dongle *dongle, long timestamp)
{
	pthread_mutex_lock(&dongle->d_lock);
	dongle->last_release = timestamp;
	dongle->is_available = true;
	pthread_cond_broadcast(&dongle->d_cond);
	pthread_mutex_unlock(&dongle->d_lock);
}
