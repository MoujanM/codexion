
#include "codex.h"

int    start_monitor(t_sim *sim, void *routine)
{
    pthread_t       *monitor;

    if (pthread_create(monitor, NULL, routine, sim))
        return (1);
    



}

void    stop_sim(t_sim *sim)
{
    int     i;

    pthread_mutex_lock(&sim->sim_lock);
    sim->stop_sim = true;

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
    int     deadline;

    deadline = sim->args->time_to_burnout;
    
    return ((timestamp - coder->last_compile > deadline) ||
    (timestamp - sim->start_time > deadline && !coder->nb_compiles))
}

void    *monitor_routine(t_sim *sim)
{
    int     all_done;
    int     i;
    long    now;
    t_coder coder;

    while (true)
    {
        all_done = 1;
        while (i < sim->args->nb_coders)
        {
            coder = sim->coders[i];
            pthread_mutex_lock(coder.lock);
            now = elapsed_time_ms(sim);
            if (coder_burnt(sim, &coder, now))
            {
                stop_sim(sim);
                print_log(&coder, now, BURNT);
                return (NULL);
            }
            if (sim->coders[i].nb_compiles < sim->args->nb_compiles_req)
                all_done = 0;
            pthread_mutex_unlock(coder.lock);
            i++;
        }
        if (all_done == 1)
        {
            stop_sim(sim);
            return (NULL);
        }
    }
    usleep(7000);
}
