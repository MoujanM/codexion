/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 15:54:02 by mmirdama          #+#    #+#             */
/*   Updated: 2026/09/29 20:10:03 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	main(int argc, char *argv[])
{
	t_args		args;

	if (ft_parser(argc, argv, &args) != 0)
	{
		fprintf(stderr, "Parsing error - aborting\n");
		return (1);
	}
	if (codexion(&args) != 0)
		return (1);
	return (0);
}
