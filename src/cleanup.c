/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:31:41 by mmirdama          #+#    #+#             */
/*   Updated: 2026/09/24 16:52:09 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	clean_dongle(t_dongle *dongle)
{
	if (!dongle)
		return (1);
	if (pthread_mutex_destroy(&dongle->d_lock))
		return (1);
	if (pthread_cond_destroy(&dongle->d_cond))
		return (1);
	return (0);
}

int	clean_coder(t_coder *coder)
{
	if (!coder)
		return (1);
	if (pthread_mutex_destroy(&coder->lock))
		return (1);
	return (0);
}

int	clean_sim(t_sim *sim)
{
	if (!sim)
		return (1);
	if (pthread_mutex_destroy(&sim->sim_lock)
		|| pthread_mutex_destroy(&sim->print_lock)
		|| pthread_mutex_destroy(&sim->seq_lock))
		return (1);
	free(sim->coders);
	free(sim->dongles);
	free(sim);
	return (0);
}

int	clean_all(t_sim *sim)
{
	// needs to use sim counters
	int	i;

	i = 0;
	while (i < sim->d_init)
	{
		clean_dongle(&sim->dongles[i]);
		i++;
	}
	i = 0;
	while (i < sim->c_init)
	{
		clean_coder(&sim->coders[i]);
		i++;
	}
	clean_sim(sim);
	return (0);
}
