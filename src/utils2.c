/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:48:01 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 18:49:24 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

bool	has_priority(t_req request1, t_req request2)
{
	if (request1.priority != request2.priority)
		return (request1.priority < request2.priority);
	return (request1.sequence < request2.sequence);
}

long	next_sequence(t_sim *sim)
{
	long	seq;

	pthread_mutex_lock(&sim->seq_lock);
	seq = sim->seq_counter++;
	pthread_mutex_unlock(&sim->seq_lock);
	return (seq);
}

bool	is_stopped(t_sim *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->sim_lock);
	stopped = sim->stop_sim;
	pthread_mutex_unlock(&sim->sim_lock);
	return (stopped);
}
