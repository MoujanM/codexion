/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:36:54 by mmirdama          #+#    #+#             */
/*   Updated: 2026/07/31 16:16:21 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int     fail_safe(int ret, char *msg)
{
    if (ret != 0)
    {
        fprintf(stderr, "Encountered Error: %s\n", msg);
        exit (1);
    }
    return (ret);
}

long     get_time_ms(void)
{
    struct timespec ts;
    
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000L + ts.tv_nsec / 1000000);
}


long    elapsed_time_ms(t_sim *sim)
{
    return (get_time_ms() - sim->start_time);
}


void    print_log(t_coder *coder, long timestamp, action act)
{
    static const char   *act_str[] = {
        "has taken a dongle",
        "is compiling",
        "is debugging",
        "is refactoring",
        "burnt out"
    };
    pthread_mutex_lock(&coder->sim->print_lock);
    printf("%ld %d %s\n", timestamp, coder->coder_id, act_str[act]);
    pthread_mutex_unlock(&coder->sim->print_lock);
}

int     get_parent(int  idx)
{
    return ((idx - 1) / 2);
}