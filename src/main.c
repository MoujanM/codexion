/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmirdama <mmirdama@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 15:54:02 by mmirdama          #+#    #+#             */
/*   Updated: 2026/08/03 18:49:32 by mmirdama         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int		main(int argc, char *argv[])
{
	// function creates simulation,
	// runs codexion func on "table"
	
	t_args		*args;

	args = NULL;

	if (ft_parser(argc, argv, args) != 0)
		return (1);	// add instructions for use
	if (codexion(args) != 0)
		return (1);
	return (0);

}