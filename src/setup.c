/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:32:39 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/24 15:48:27 by mmirdama         ###   ########.fr       */
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
		coders[i].coder_id = i + 1;
		coders[i].last_compile = 0;
		coders[i].nb_compiles = 0;
		if (i == n - 1)
		{
			coders[i].dongle_1 = &dongles[(i + 1) % n];
			coders[i].dongle_2 = &dongles[i];
		}
		else
		{
			coders[i].dongle_1 = &dongles[i];
			coders[i].dongle_2 = &dongles[(i + 1) % n];
		}
		coders[i].sim = sim;
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

bool	is_stopped(t_sim *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->sim_lock);
	stopped = sim->stop_sim;
	pthread_mutex_unlock(&sim->sim_lock);
	return (stopped);
}

long	next_sequence(t_sim *sim)
{
	long	seq;

	pthread_mutex_lock(&sim->seq_lock);
	seq = sim->seq_counter++;
	pthread_mutex_unlock(&sim->seq_lock);
	return (seq);
}
