
#ifndef CODEX_H
 #define CODEX_H

 #include <pthread.h>
 #include <limits.h>
 #include <string.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <time.h>
 

 #define MAX 100      //arbitrary for heap


typedef enum      actions
{
    GRAB, COMPILE, DEBUG, REFACTOR
} action;

// the info-box
typedef struct  s_simulation
{
    int             start_time;
    int             current_time;
    t_args          *args;
    t_coder         *coders;
    t_dongle        *dongles;
    pthread_mutex_t print_lock;      //to guard logging
    pthread_mutex_t sim_lock;       //to guard box-level info
    pthread_cond_t  print_lock;
    bool            stop_sim;       //check if burnout or required compiles reached

} t_sim;


 // parsing struct
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

// define structs for coders & dongles

typedef struct  s_dongle
{
    int             dongle_id;
    bool            is_available;
    long            last_release;
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
    action          act; //maybe just a burntout flag
    t_sim           *sim;

} t_coder;

typedef struct  s_request
{
    int     coder_id;
    int     priority;       //need something here - assume will be filled with fifo/edf
} t_request;

typedef struct  s_heap
{
    t_request   request[MAX];
    int         size;
} t_heap;

// parsing and set-up
int     ft_parser(int ac, char **av, t_args *args);
void	fill_args_struct(int *arg_arr, char *scheduler, t_args *args);
t_sim   *create_simulation(t_args *args);
void    init_coders(t_sim *sim);
void    init_dongles(t_sim *sim);

