/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:37:17 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/03 18:17:59 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void    stop_sim(t_sim *sim)
{
    int     i;

    pthread_mutex_lock(&sim->sim_lock);
    sim->stop_sim = true;
    pthread_mutex_unlock(&sim->sim_lock);
    
    i = 0;
    while (i < sim->args->nb_coders)
    {
        pthread_mutex_lock(&sim->dongles[i].d_lock);
        pthread_cond_broadcast(&sim->dongles[i].d_cond);
        pthread_mutex_unlock(&sim->dongles[i].d_lock);
        i++;
    }
}

bool    coder_burnt(t_sim *sim, t_coder *coder, long timestamp)
{
    return (timestamp - coder->last_compile > sim->args->time_to_burnout);
}

void    *monitor_routine(void *arg)
{
    int     all_done;
    int     i;
    long    now;
    t_sim   *sim;

    sim = (t_sim *)arg;
    while (true)
    {
        all_done = 1;
        i = 0;
        while (i < sim->args->nb_coders)
        {
            pthread_mutex_lock(&sim->coders[i].lock);
            now = elapsed_time_ms(sim);
            if (coder_burnt(sim, &sim->coders[i], now))
            {
                pthread_mutex_unlock(&sim->coders[i].lock);
                print_log(&sim->coders[i], now, BURNT);
                stop_sim(sim);
                return (NULL);
            }
            if (sim->coders[i].nb_compiles < sim->args->nb_compiles_req)
                all_done = 0;
            pthread_mutex_unlock(&sim->coders[i].lock);
            i++;
        }
        if (all_done == 1)
        {
            stop_sim(sim);
            return (NULL);
        }
        usleep(3000);
    }
}
