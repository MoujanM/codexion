/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:32:39 by mmirdama          #+#    #+#             */
/*   Updated: 2026/09/24 16:46:45 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	init_dongles(t_sim *sim)
{
	t_dongle	*dongles;
	int			i;

	dongles = sim->dongles;
	i = 0;
	while (i < sim->args->nb_coders)
	{
		if (pthread_mutex_init(&dongles[i].d_lock, NULL) != 0)
			return (1);
		if (pthread_cond_init(&dongles[i].d_cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongles[i].d_lock);
			return (1);
		}
		dongles[i].dongle_id = i + 1;
		dongles[i].is_available = true;
		dongles[i].last_release = 0;
		dongles[i].cooldown = sim->args->dongle_cooldown;
		dongles[i].request_heap.size = 0;
		i++;
		sim->d_init = i;
	}
	return (0);
}

static void	init_single_coder(t_sim *sim, int i)
{
	t_coder	*coder;
	int		n;

	coder = &sim->coders[i];
	n = sim->args->nb_coders;
	coder->coder_id = i + 1;
	coder->last_compile = 0;
	coder->nb_compiles = 0;
	if (n == 1)
	{
		coder->dongle_1 = &sim->dongles[i];
		coder->dongle_2 = NULL;
	}
	else if (i == n - 1)
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
	int			i;

	coders = sim->coders;
	i = 0;
	while (i < sim->args->nb_coders)
	{
		init_single_coder(sim, i);
		if (pthread_mutex_init(&coders[i].lock, NULL) != 0)
		{
			fprintf(stderr, "Coder mutex creation failed");
			return (1);
		}
		if (pthread_create(&coders[i].thread, NULL, coder_routine, (void *)&coders[i]) != 0)
		{
			pthread_mutex_destroy(&coders[i].lock);
			fprintf(stderr, "Coder thread creation failed");
			return(1);
		}
		i++;
		sim->c_init = i;
	}
	return (0);
}

static int	init_sim_locks(t_sim *sim)
{
	if (pthread_mutex_init(&sim->print_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->sim_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_lock);
		return (1);
	}
	if (pthread_mutex_init(&sim->seq_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_lock);
		pthread_mutex_destroy(&sim->sim_lock);
		return (1);
	}
	if (pthread_cond_init(&sim->s_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_lock);
		pthread_mutex_destroy(&sim->sim_lock);
		pthread_mutex_destroy(&sim->seq_lock);
		return (1);
	}
	return (0);
}

static int		set_sim_opts(t_sim *sim, int nb)
{
	sim->coders_ready = false;
	sim->start_time = get_time_ms();
	sim->seq_counter = 0;
	sim->stop_sim = false;
	sim->coders = malloc(sizeof(t_coder) * nb);
	sim->dongles = malloc(sizeof(t_dongle) * nb);
	if (!sim->coders || !sim->dongles)
	{
		free(sim->coders);
		free(sim->dongles);
		free(sim);
		return (1);
	}
	return (0);
}
t_sim	*create_simulation(t_args *args)
{
	t_sim		*sim;
	int			nb;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	memset(sim, 0, sizeof(t_sim));
	sim->args = args;
	nb = args->nb_coders;
	if (set_sim_opts(sim, nb) != 0)
	{
		fprintf(stderr, "Malloc Failure");
		return (NULL);
	}
	if (init_sim_locks(sim) != 0)
	{
		fprintf(stderr, "Failed to initiate sim primitives");
		return (NULL);
	}
	return (sim);
}
