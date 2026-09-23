/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 16:05:29 by mmirdama          #+#    #+#             */
/*   Updated: 2026/09/23 18:49:38 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	init_simulation(t_sim *sim)
{
	if (init_dongles(sim) != 0 || init_coders(sim, coder_routine) != 0)
		return ;
	if (!sim->coders_ready)
	{
		pthread_mutex_lock(&sim->sim_lock);
		sim->coders_ready = true;
		sim->start_time = get_time_ms();
		pthread_cond_broadcast(&sim->s_cond);
		pthread_mutex_unlock(&sim->sim_lock);
	}
	if (fail_safe(pthread_create(&sim->monitor, NULL, monitor_routine, sim),
		"Monitor thread creation failed", sim) != 0)
		return ;
	sim->monitor_init = true;
}

void	join_coders(t_coder *coders, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		if (coders[i].thread_init)
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
	if (sim->coders_init > 0)
		join_coders(sim->coders, sim->coders_init);
	if (sim->monitor_init)
		pthread_join(sim->monitor, NULL);
	if (is_stopped(sim))
	{
		clean_all(sim);
		return (0);
	}
	clean_all(sim);
	return (1);
}
