/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:32:39 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/25 19:10:31 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	init_dongles(t_sim *sim)
{
	t_dongle	*dongles;
	int			n;
	int			i;
	int			ret;

	dongles = sim->dongles;
	n = sim->args->nb_coders;
	i = 0;
	while (i <= n - 1)
	{
		dongles[i].dongle_id = i + 1;
		dongles[i].is_available = true;
		dongles[i].last_release = 0;
		dongles[i].cooldown = sim->args->dongle_cooldown;
		pthread_mutex_init(&dongles[i].d_lock, NULL);
		ret = pthread_cond_init(&dongles[i].d_cond, NULL);
		fail_safe(ret, "Mutex cond creation failed.");
		dongles[i].request_heap.size = 0;
		i++;
	}
	return (0);
}

static void	init_single_coder(t_sim *sim, int i, void *coder_routine)
{
	t_coder	*coder;
	int		n;

	coder = &sim->coders[i];
	n = sim->args->nb_coders;
	coder->coder_id = i + 1;
	coder->last_compile = 0;
	coder->nb_compiles = 0;
	if (i == n - 1)
	{
		coder->dongle_1 = &sim->dongles[(i + 1) % n];
		coder->dongle_2 = &sim->dongles[i];
	}
	else
	{
		coder->dongle_1 = &sim->dongles[i];
		coder->dongle_2 = &sim->dongles[(i + 1) % n];
	}
	coder->sim = sim;
}

int	init_coders(t_sim *sim, void *coder_routine)
{
	t_coder		*coders;
	t_dongle	*dongles;
	int			n;
	int			i;

	coders = sim->coders;
	dongles = sim->dongles;
	n = sim->args->nb_coders;
	i = 0;
	while (i < n)
	{
		init_single_coder(sim, i, coder_routine);
		fail_safe(pthread_mutex_init(&coders[i].lock, NULL),
			"Coder mutex failed.");
		fail_safe(pthread_create(&coders[i].thread, NULL,
				coder_routine, (void *)&coders[i]), "Thread creation failed.");
		i++;
	}
	return (0);
}

t_sim	*create_simulation(t_args *args)
{
	t_sim		*sim;
	t_coder		*coders;
	t_dongle	*dongles;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	sim->coders_ready = false;
	sim->start_time = get_time_ms();
	sim->seq_counter = 0;
	sim->args = args;
	coders = malloc(sizeof(t_coder) * args->nb_coders);
	if (!coders)
		return (NULL);
	dongles = malloc(sizeof(t_dongle) * args->nb_coders);
	if (!dongles)
		return (NULL);
	sim->coders = coders;
	sim->dongles = dongles;
	sim->stop_sim = false;
	fail_safe(pthread_mutex_init(&sim->print_lock, NULL), "Sim Mutex failed");
	fail_safe(pthread_mutex_init(&sim->sim_lock, NULL), "Sim Mutex failed");
	fail_safe(pthread_mutex_init(&sim->seq_lock, NULL), "Sim Mutex failed");
	fail_safe(pthread_cond_init(&sim->s_cond, NULL), "Sim Cond failed");
	return (sim);
}
