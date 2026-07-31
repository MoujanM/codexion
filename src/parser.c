
#include "codex.h"


void	fill_args_struct(int *arg_arr, char *scheduler, t_args *args)
{
	args->nb_coders = arg_arr[0];
	args->time_to_burnout = arg_arr[1];
	args->time_to_compile = arg_arr[2];
	args->time_to_debug = arg_arr[3];
	args->time_to_refactor = arg_arr[4];
	args->nb_compiles_req = arg_arr[5];
	args->dongle_cooldown = arg_arr[6];
	args->scheduler = scheduler;
}

int    ft_parser(int ac, char **av, t_args *args)
{
	int		arg_arr[7];
	int		i;
	int		n;

	if (ac != 9)
		return (1);
	i = 1;
	while (i < ac - 1)
	{
		n = atoi(av[i]);
		if (n == -1)
			return (11);
		arg_arr[i - 1] = n;
		i++;
	}
	if (strcmp(av[i], "fifo") && strcmp(av[i], "edf"))
		return (1);

	fill_args_struct(arg_arr, av[i], args);
	return (0);
}