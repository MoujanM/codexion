/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 16:05:29 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 18:55:48 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	init_simulation(t_sim *sim)
{
	if (init_dongles(sim) != 0)
	{
		fprintf(stderr, "Dongle init failed.");
		return (1);
	}
	if (init_coders(sim, coder_routine) != 0)
	{
		fprintf(stderr, "Coder init failed.");
		return (1);
	}
	if (!sim->coders_ready)
	{
		pthread_mutex_lock(&sim->sim_lock);
		sim->coders_ready = true;
		sim->start_time = get_time_ms();
		pthread_cond_broadcast(&sim->s_cond);
		pthread_mutex_unlock(&sim->sim_lock);
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
	{
		fprintf(stderr, "Monitor init failed.");
		return (1);
	}
	return (0);
}

void	join_coders(t_coder *coders, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
}

int	codexion(t_args *args)
{
	t_sim	*sim;

	sim = create_simulation(args);
	if (!sim)
		return (1);
	if (init_simulation(sim) != 0)
		return (1);
	join_coders(sim->coders, args->nb_coders);
	pthread_join(sim->monitor, NULL);
	if (is_stopped(sim))
	{
		clean_all(sim);
		return (0);
	}
	return (1);
}
