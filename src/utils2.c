/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:48:01 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/27 16:45:44 by mmirdama         ###   ########.fr       */
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

void	*lone_coder(t_coder *coder, t_dongle *dongle)
{
	long	t_sleep;

	t_sleep = coder->sim->args->time_to_burnout;
	pthread_mutex_lock(&dongle->d_lock);
	dongle->is_available = false;
	pthread_mutex_unlock(&dongle->d_lock);
	print_log(coder, elapsed_time_ms(coder->sim), GRAB);
	usleep(t_sleep * 1000);
	return (NULL);
}
