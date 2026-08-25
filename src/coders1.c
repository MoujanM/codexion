/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:49 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 18:34:49 by mmirdama         ###   ########.fr       */
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

void	register_requests(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_req	request;

	request = make_request(coder);
	lock_pair(first, second);
	insert(&first->request_heap, request);
	insert(&second->request_heap, request);
	unlock_pair(first, second);
}

void	release_dongle(t_dongle *dongle, long timestamp)
{
	pthread_mutex_lock(&dongle->d_lock);
	dongle->last_release = timestamp;
	dongle->is_available = true;
	pthread_cond_broadcast(&dongle->d_cond);
	pthread_mutex_unlock(&dongle->d_lock);
}

void	lock_pair(t_dongle *a, t_dongle *b)
{
	if (a->dongle_id < b->dongle_id)
	{
		pthread_mutex_lock(&a->d_lock);
		pthread_mutex_lock(&b->d_lock);
	}
	else
	{
		pthread_mutex_lock(&b->d_lock);
		pthread_mutex_lock(&a->d_lock);
	}
}

void	unlock_pair(t_dongle *a, t_dongle *b)
{
	pthread_mutex_unlock(&a->d_lock);
	pthread_mutex_unlock(&b->d_lock);
}
