/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 23:21:03 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/10 23:36:16 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

int main()
{
	int		fd[2];
	char	string[8];

	if (pipe(fd) != 0)
	{
		printf("Deu ruim\n");
		return (1);
	}

	write(fd[1], "Bom dia\0", 8);

	read(fd[0], string, 8);

	printf("Lido do fd[0] = %s\n", string);

	close(fd[0]);
	close(fd[1]);

	return (0);
}
