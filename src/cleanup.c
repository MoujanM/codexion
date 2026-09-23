/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:31:41 by mmirdama          #+#    #+#             */
/*   Updated: 2026/09/23 18:49:38 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	clean_dongle(t_dongle *dongle)
{
	if (!dongle)
		return (1);
	if (dongle->d_lock_init && pthread_mutex_destroy(&dongle->d_lock))
		return (1);
	if (dongle->d_cond_init && pthread_cond_destroy(&dongle->d_cond))
		return (1);
	return (0);
}

int	clean_coder(t_coder *coder)
{
	if (!coder)
		return (1);
	if (coder->lock_init && pthread_mutex_destroy(&coder->lock))
		return (1);
	return (0);
}

int	clean_sim(t_sim *sim)
{
	if (!sim)
		return (1);
	if (sim->sim_lock_init && pthread_mutex_destroy(&sim->sim_lock))
		return (1);
	if (sim->print_lock_init && pthread_mutex_destroy(&sim->print_lock))
		return (1);
	if (sim->seq_lock_init && pthread_mutex_destroy(&sim->seq_lock))
		return (1);
	if (sim->s_cond_init && pthread_cond_destroy(&sim->s_cond))
		return (1);
	free(sim->coders);
	free(sim->dongles);
	free(sim);
	return (0);
}

int	clean_all(t_sim *sim)
{
	int	i;

	if (!sim)
		return (1);
	i = -1;
	while (++i < sim->coders_init)
		if (sim->coders[i].thread_init)
			pthread_join(sim->coders[i].thread, NULL);
	i = -1;
	while (++i < sim->coders_init)
		clean_coder(&sim->coders[i]);
	i = -1;
	while (++i < sim->dongles_init)
		clean_dongle(&sim->dongles[i]);
	return (clean_sim(sim));
}
