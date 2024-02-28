/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 22:50:00 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/27 23:37:39 by danbarbo         ###   ########.fr       */
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
	char	*args[] = {"../pipex/pipex", "5", NULL};

	printf("Começando\n");
	pid = fork();
	if (pid == 0)
	{
		execve(args[0], args, envp);
		return (1);

// 		char *str;
//
// 		if (errno == EACCES)
// 			str = ""
//
// 		perror("Deu ruim");
	}
	puts("Esperando");
	waitpid(pid, NULL, 0);
	printf("Acabou - %d\n", pid);
	return (0);
}
