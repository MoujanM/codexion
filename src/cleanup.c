
#include "codex.h"

// need to destroy all mutexes
// exit all threads

int     clean_dongle(t_dongle *dongle)
{
    if (!dongle)
        return (1);
    if (pthread_mutex_destroy(&dongle->d_lock))
        return (1);
    if (pthread_cond_destroy(&dongle->d_cond))
        return (1);
    return (0);
}

int     clean_coder(t_coder *coder)
{
    if (!coder)
        return (1);
    if (pthread_mutex_destroy(&coder->lock))
        return (1);
    return (0);
}

int     clean_sim(t_sim *sim)
{
    if (!sim)
        return (1);
    if (pthread_mutex_destroy(&sim->sim_lock) ||
    pthread_mutex_destroy(&sim->print_lock))
        return (1);
    free(sim->coders);
    free(sim->dongles);
    return (0);
}

int     clean_all(t_dongle *dongle, t_coder *coder, t_sim *sim)
{
    int     n;
    int     i;

    n = sim->args->nb_coders;
    i = 0;

    while (n > 0)
    {
        clean_dongle(sim->dongles[i]);
        clean_coder(sime->coders[i]);
        i++;
        n--;
    }
    clean_sim(sim);
    return (0);
}
