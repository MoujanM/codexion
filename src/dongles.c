/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:49 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/24 19:20:38 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static void  pick_up(t_dongle *first, t_dongle *second)
{
    pthread_mutex_lock(&first->d_lock);
    pthread_mutex_lock(&second->d_lock);
    pop_min(&first->request_heap);
    pop_min(&second->request_heap);
    first->is_available = false;
    second->is_available = false;
    pthread_mutex_unlock(&first->d_lock);
    pthread_mutex_unlock(&second->d_lock);
}

int     take_if_available(t_coder *coder, t_dongle *first, t_dongle *second)
{
    long    t_wait;
    
    if (is_stopped(coder->sim))
        return (1);
    // pthread_mutex_lock(&first->d_lock);
    // pthread_mutex_lock(&second->d_lock);
    if (is_ready(first, coder) && is_ready(second, coder))
    {
        pick_up(first, second);
        return (0);
    }
    else 
    {
        t_wait = remaining_cooldown
        if (!is_ready(first, coder))
        // pthread_mutex_unlock(&second->d_lock);
        pthread_mutex_lock(&first->d_lock);
        pthread_cond_wait(&first->d_cond, &first->d_lock);
        pthread_mutex_unlock(&first->d_lock);
    }
    else
    {
        // pthread_mutex_unlock(&first->d_lock);
        pthread_mutex_lock(&second->d_lock);
        pthread_cond_wait(&second->d_cond, &second->d_lock);
        pthread_mutex_unlock(&second->d_lock);
    }
    return (1);
}

bool    is_ready(t_dongle *dongle, t_coder *coder)
{
    bool    cooled;

    cooled = (dongle->last_release == 0
            || get_time_ms() >= dongle->last_release + dongle->cooldown);
    return (dongle->is_available && cooled && dongle->request_heap.request[0].coder_id == coder->coder_id);
}

void    register_requests(t_coder *coder, t_dongle *first, t_dongle *second)
{
    t_req	request;
    
    request = make_request(coder);
    pthread_mutex_lock(&first->d_lock);
    insert(&first->request_heap, request);
    pthread_mutex_unlock(&first->d_lock);
    pthread_mutex_lock(&second->d_lock);
    insert(&second->request_heap, request);
    pthread_mutex_unlock(&second->d_lock);
}

long    remaining_cooldown(t_dongle *dongle)
{
    long    to_wait;

    pthread_mutex_lock(&dongle->d_lock);
    if (dongle->last_release != 0)
    {
        to_wait = (dongle->last_release + dongle->cooldown) - get_time_ms();
        pthread_mutex_unlock(&dongle->d_lock);
        return (to_wait);
    }
    pthread_mutex_unlock(&dongle->d_lock);
    return (0);
}

// void    wait_for_cooldown(t_dongle *first, t_dongle *second)
// {
//     long    max_wait;
//     long    first_w;
//     long    second_w;

//     first_w = remaining_cooldown(first);
//     second_w = remaining_cooldown(second);
//     max_wait = first_w;
//     if (max_wait < second_w)
//         max_wait = second_w;
//     if (max_wait <= 0)
//         max_wait = 0;
//     usleep(max_wait * 1000);
// }

int	grab_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

    if (is_stopped(coder->sim))
		return (1);
    first = coder->dongle_1;
	second = coder->dongle_2;
    register_requests(coder, first, second);
    while (!is_stopped(coder->sim))
    {
        // if (is_stopped(coder->sim))
        //     return (1);
        // wait_for_cooldown(first, second);
        if (take_if_available(coder, first, second) == 0)
        {
            print_log(coder, elapsed_time_ms(coder->sim), GRAB);
            break ;
        }
    } 
    return (0);
}
