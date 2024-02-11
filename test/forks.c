/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 22:38:49 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/10 22:50:12 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

int main()
{
	int pid;

	puts("Comaçando!");

	pid = fork();

	if (pid != 0)
	{
		printf("%5d - Bom dia\n", pid);
		pid = fork();
	}
	else
		printf("%5d - Batata\n", pid);

	printf("%5d - Alo\n", pid);


	return (0);
}
