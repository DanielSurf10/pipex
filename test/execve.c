/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 22:50:00 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/12 19:18:03 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[], char *envp[])
{
	int		pid;
	char	*args[] = {"/bin", "echo batata", NULL};

	printf("Começando\n");
	pid = fork();
	if (pid == 0)
	{
		execve("/bin", args, envp);

		char *str;

		// if (errno == EACCES)
		// 	str = ""

		perror("Deu ruim");
	}
	else
	{
		puts("Esperando");
		wait(NULL);
	}
	printf("Acabou - %d\n", pid);
	return (0);
}
