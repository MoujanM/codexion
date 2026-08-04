/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/31 13:38:39 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/04 19:07:00 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
 #define CODEX_H

 #include <pthread.h>
 #include <limits.h>
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <stdbool.h>
 #include <time.h>
//  #include <linux/time.h>

 #define MAX 100      //arbitrary for heap
 
typedef struct s_args       t_args;
typedef struct s_coder      t_coder;
typedef struct s_dongle     t_dongle;
typedef struct s_simulation t_sim;

typedef struct  s_request
{
    int      coder_id;
    long     priority;
} t_request;

typedef struct  s_heap
{
    t_request   request[MAX];
    int         size;
} t_heap;

// the info-box
typedef struct  s_simulation
{
    bool            coders_ready;
    long            start_time;
    t_args          *args;
    t_coder         *coders;
    t_dongle        *dongles;
    pthread_t       monitor;
    pthread_mutex_t print_lock;      //to guard logging
    pthread_mutex_t sim_lock;       //to guard box-level info
    pthread_cond_t  s_cond;
    bool            stop_sim;       //check if burnout or required compiles reached

} t_sim;

typedef struct  s_args
{
    int     nb_coders;
    int     time_to_burnout;
    int     time_to_compile;
    int     time_to_debug;
    int     time_to_refactor;
    int     nb_compiles_req;
    int     dongle_cooldown;
    char    *scheduler;
} t_args;

typedef struct  s_dongle
{
    int             dongle_id;
    bool            is_available;
    long            last_release;
    int             cooldown;
    pthread_mutex_t d_lock;
    pthread_cond_t  d_cond;
    t_heap          request_heap;

} t_dongle;

typedef struct  s_coder
{
    int             coder_id;
    long            last_compile;   //start of last_compile
    int             nb_compiles;
    pthread_t       thread;
    t_dongle        *dongle_1;
    t_dongle        *dongle_2;
    pthread_mutex_t lock;
    t_sim           *sim;

} t_coder;

typedef enum      actions
{
    GRAB, COMPILE, DEBUG, REFACTOR, BURNT
} action;

typedef struct  s_monitor
{
    pthread_t   thread;
    t_sim       *sim;
} t_monitor;

// parsing and set-up
int     ft_parser(int ac, char **av, t_args *args);
void	fill_args_struct(int *arg_arr, char *scheduler, t_args *args);

t_sim   *create_simulation(t_args *args);
void		init_simulation(t_sim *sim);
int     init_dongles(t_sim *sim);
int     init_coders(t_sim *sim, void *coder_routine);
void	join_coders(t_coder *coders, int n);
int		codexion(t_args *args);

long     get_time_ms(void);
long     elapsed_time_ms(t_sim *sim);
void     print_log(t_coder *coder, long timestamp, action act);

//coder actions
t_request	make_request(t_coder *coder);
int         request_or_grab(t_coder *coder, t_dongle *dongle);
int         grab_dongles(t_coder *coder);
void        release_dongle(t_dongle *dongle, long timestamp);

void        compile(t_coder *coder);
void        debug(t_coder *coder);
void        refactor(t_coder *coder);
void        *coder_routine(void *args);

//simulation management
void    print_log(t_coder *coder, long timestamp, action act);
long    get_time_ms(void);
long    elapsed_time_ms(t_sim *sim);
int     fail_safe(int ret, char *msg);

// mini-heap
void    swap(t_request *x, t_request *y);
void    bubble_up(t_heap *heap, int idx);
void    bubble_down(t_heap *heap, int idx);
void    insert(t_heap *heap, t_request request);
void    pop_min(t_heap *heap);
int     get_parent(int  idx);

// monitor
void    stop_sim(t_sim *sim);
bool    coder_burnt(t_sim *sim, t_coder *coder, long timestamp);
void    *monitor_routine(void *arg);

// cleanup
int     clean_dongle(t_dongle *dongle);
int     clean_coder(t_coder *coder);
int     clean_sim(t_sim *sim);
int     clean_all(t_sim *sim);
#endif