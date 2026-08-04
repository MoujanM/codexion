/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:58:39 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/04 19:19:44 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void    compile(t_coder *coder)
{
    long    now;
    long    compile_time;

    now = elapsed_time_ms(coder->sim);
    compile_time = coder->sim->args->time_to_compile;
    print_log(coder, now, COMPILE);

    pthread_mutex_lock(&coder->lock);
    coder->last_compile = now;
    pthread_mutex_unlock(&coder->lock);
    
    usleep(compile_time * 1000);
    release_dongle(coder->dongle_1, get_time_ms());
    release_dongle(coder->dongle_2, get_time_ms());
    
    pthread_mutex_lock(&coder->lock);
    coder->nb_compiles += 1;
    pthread_mutex_unlock(&coder->lock);
}

void    debug(t_coder *coder)
{
    long    now;
    long    debug_time;

    now = elapsed_time_ms(coder->sim);
    print_log(coder, now, DEBUG);
    debug_time = coder->sim->args->time_to_debug;
    usleep(debug_time * 1000);

}
void    refactor(t_coder *coder)
{
    long    now;
    long    refactor_time;

    now = elapsed_time_ms(coder->sim);
    print_log(coder, now, REFACTOR);
    refactor_time = coder->sim->args->time_to_refactor;
    usleep(refactor_time * 1000);
}

void    *coder_routine(void *args)
{
	t_coder *coder;
	
	coder = (t_coder *)args;
    while (!coder->sim->coders_ready)
        pthread_cond_wait(&coder->sim->s_cond, &coder->sim->sim_lock);
    while (!coder->sim->stop_sim)
    {
        if (grab_dongles(coder) == 0)
        {
            compile(coder);
            debug(coder);
            refactor(coder);
        }
    }
    return (NULL);
}
