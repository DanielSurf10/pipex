/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_in_two_processes.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 23:36:34 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/20 19:03:27 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int	fd[2];
	int	pid;

	if (pipe(fd) != 0)
	{
		printf("Deu ruim\n");
		return (1);
	}

	pid = fork();

	if (pid == 0)
	{
		close(fd[0]);			// Fecha o fd de leitura pq não precisa aqui
		int x = 1503;
		write(fd[1], &x, sizeof(int));
		close(fd[1]);			// Depois de escrever fecha o fd
		return (0);
	}
	close(fd[1]);			// Fecha o fd de escrita pq não precisa aqui
	int y;
	wait(NULL);
	read(fd[0], &y, sizeof(int));
	close(fd[0]);			// Depois de ler fecha o fd
	printf("O número é: %d\n", y);
	printf("Acabou - pid: %d\n", pid);

	return (0);
}
