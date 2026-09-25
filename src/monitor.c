/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:37:17 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/24 16:30:52 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	stop_sim(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->sim_lock);
	sim->stop_sim = true;
	pthread_mutex_unlock(&sim->sim_lock);
	i = 0;
	while (i < sim->d_init)
	{
		pthread_mutex_lock(&sim->dongles[i].d_lock);
		pthread_cond_broadcast(&sim->dongles[i].d_cond);
		pthread_mutex_unlock(&sim->dongles[i].d_lock);
		i++;
	}
}

bool	coder_burnt(t_sim *sim, t_coder *coder, long timestamp)
{
	return (timestamp - coder->last_compile > sim->args->time_to_burnout);
}

int	check_coder(t_sim *sim, t_coder *coder)
{
	long	now;

	now = elapsed_time_ms(sim);
	pthread_mutex_lock(&coder->lock);
	if (coder_burnt(sim, coder, now))
	{
		pthread_mutex_unlock(&coder->lock);
		print_log(coder, now, BURNT);
		return (1);
	}
	if (coder->nb_compiles < sim->args->nb_compiles_req)
	{
		pthread_mutex_unlock(&coder->lock);
		return (2);
	}
	pthread_mutex_unlock(&coder->lock);
	return (0);
}

int	sweep(t_sim *sim)
{
	int	all_done;
	int	i;
	int	status;

	all_done = 1;
	i = 0;
	while (i < sim->args->nb_coders)
	{
		status = check_coder(sim, &sim->coders[i]);
		if (status == 1)
			return (1);
		if (status == 2)
			all_done = 0;
		i++;
	}
	return (all_done);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		all_done;

	sim = (t_sim *)arg;
	while (true)
	{
		all_done = sweep(sim);
		if (all_done == 1)
		{
			stop_sim(sim);
			return (NULL);
		}
		usleep(3000);
	}
}
