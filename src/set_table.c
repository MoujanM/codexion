
#include "codex.h"

void    init_dongles(t_sim *sim)
{
    t_dongle    *dongles;
    int         n;
    int         i;
    
    dongles = sim->dongles;
    n = sim->args->nb_coders;
    i = 0;
    while (i <= n - 1)
    {
        dongles[i].dongle_id = i + 1;
        dongles[i].is_available = true;
        pthread_mutex_init(&dongles[i].lock, NULL);
        i++;
    }
}

void    init_coders(t_sim *sim)
{
    t_coder     *coders;
    t_dongle    *dongles;
    int         n;
    int         i;

    coders = sim->coders;
    dongles = sim->dongles;
    n = sim->args->nb_coders;
    i = 0;
    while (i <= n - 1)
    {
        coders[i].coder_id = i + 1;
        coders[i].last_compile = 
        coders[i].dongle_1 = &dongles[i];
        coders[i].dongle_2 = &dongles[(i + 1) % n]     //circular access
        coders[i].sim_args = sim;
        i++;
    }
}

t_sim   *create_simulation(t_args *args)
{
    t_sim       *sim;
    t_coder     *coders;
    t_dongle    *dongles;

    sim = malloc(sizeof(t_sim));
    if (!sim)
        return NULL;
    sim->current_time = gettimeofday();
    sim->args = args;

    coders = malloc(sizeof(t_coder) * args->nb_coders);
    if !coders
        return NULL;
    dongles = malloc(sizeof(t_dongle) * args->nb_coders);
    if (!dongles)
        return NULL;
    sim->coders = coders;
    sim->dongles = dongles;
    init_dongles(sim);
    init_coders(sim);
    pthread_mutex_init(&sim->print_lock, NULL);
    pthread_mutex_init(&sim->sim_lock, NULL);
    sim->stop_sim = false;
    return (sim);
}