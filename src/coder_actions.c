
#include "codex.h"

int    request_or_grab(t_coder *coder, t_dongle *dongle)
{
    t_request           request;
    long                to_cooldown;
    struct timespec     t;

    request.coder_id = coder->coder_id;
    if (strcmp(coder->sim->args->scheduler, "fifo") == 0)
        request.priority = get_time_ms();
    if (strcmp(coder->sim->args->scheduler, "edf") == 0)
        request.priority = coder->sim->args->time_to_burnout + coder->last_compile;

    pthread_mutex_lock(&dongle->d_lock);

    insert(&dongle->request_heap, request);
    while (!(dongle->is_available && dongle->request_heap.request[0].coder_id == coder->coder_id))
    {
        to_cooldown = dongle->last_release + coder->sim->args->dongle_cooldown;
        t.tv_sec = to_cooldown / 1000;
        t.tv_nsec = (to_cooldown % 1000) * 1000000;
        pthread_cond_timedwait(&dongle->d_cond, &dongle->d_lock, &t);
    }
        
    pop_min(&dongle->request_heap);
    dongle->is_available = false;
    pthread_mutex_unlock(&dongle->d_lock);
}

int    grab_dongles(t_coder *coder)
{   // avoiding deadlock
    // minor tradeoff when odd nb_coders
    int     id;

    id = coder->code_id;
    if (id % 2 != 0)
    {
        request_or_grab(coder, coder->dongle_1);
        print_log(coder, elapsed_time_ms(coder->sim), GRAB);
        request_or_grab(coder, coder->dongle_2);
        print_log(coder, elapsed_time_ms(coder->sim), GRAB);
        return (0);
    }
    else 
    {
        request_or_grab(coder, coder->dongle_2);
        print_log(coder, elapsed_time_ms(coder->sim), GRAB);
        request_or_grab(coder, coder->dongle_1);
        print_log(coder, elapsed_time_ms(coder->sim), GRAB);
    }
    
}

void    compile(t_coder *coder)
{
    long    now;
    long    compile_time;

    now = elapsed_time_ms(coder->sim);
    compile_time = coder->sim->args->time_to_compile;
    print_log(coder, now, COMPILE);

    pthread_mutex_lock(&coder->lock);
    coder->last_compile = now;
    coder->act = COMPILE;
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

    now = get_time_ms();
    print_log(coder, now, DEBUG);
    //unlock and change action ???
    debug_time = coder->sim->args->time_to_debug;
    usleep(debug_time * 1000);

}

void    refactor(t_coder *coder)
{
    long    now;
    long    refactor_time;

    now = get_time_ms();
    print_log(coder, now, REFACTOR);
    //unlock and change action ???
    refactor_time = coder->sim->args->time_to_refactor;
    usleep(refactor_time * 1000);
}


void    release_dongle(t_dongle *dongle, long timestamp)
{
    pthread_mutex_lock(&dongle->d_lock);
    dongle->last_relase = timestamp;
    dongle->is_available = true;
    pthread_cond_broadcast(&dongle->d_cond);
    pthread_mutex_unlock(&dongle->d_lock);
}

void    *coder_routine(t_coder *coder)
{
    while (!coder->sim->stop_sim)
    {
        grab_dongles(coder);
        compile(coder);
        debug(coder);
        refactor(coder);
    }
}
