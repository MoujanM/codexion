/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 16:05:29 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/07 13:50:25 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	init_simulation(t_sim *sim)
{
	init_dongles(sim);
	if (init_coders(sim, coder_routine) == 0 && !sim->coders_ready)
	{
		pthread_mutex_lock(&sim->sim_lock);
		sim->coders_ready = true;
		sim->start_time = get_time_ms();
		pthread_cond_broadcast(&sim->s_cond);
		pthread_mutex_unlock(&sim->sim_lock);
	}
	fail_safe(pthread_create(&sim->monitor, NULL, monitor_routine, sim),
		"Monitor thread creation failed");
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
	init_simulation(sim);
	join_coders(sim->coders, args->nb_coders);
	pthread_join(sim->monitor, NULL);
	if (sim->stop_sim)
	{
		clean_all(sim);
		return (0);
	}
	return (1);
}
