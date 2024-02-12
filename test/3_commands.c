/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_pipex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/12 12:06:18 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
# include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{
	int	fd_file_in;
	int	fd_file_out;
	int	fd[2];
	int	fd2[2];
	int	pid[3];

	if (argc != 6)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <cmd> <file_out>\n", 71);
		return (1);
	}

	fd_file_in = open(argv[1], O_RDONLY);

	if (fd_file_in < 0)
		perror("Input file error");

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd_file_out < 0)
		perror("Output file error");

	pipe(fd);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		// Processo filho primeiro comando
		close(fd[0]);
		close(fd_file_out);

		char *args[] = {argv[2], NULL};

		dup2(fd_file_in, STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);

		close(fd[1]);
		close(fd_file_in);

		if (!(fd_file_in < 0) && !(fd_file_out < 0))
			execve(argv[2], args, envp);
		write(2, "Deu ruim 1\n", 11);
		exit(2);
	}

	pipe(fd2);
	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		close(fd[1]);
		close(fd2[0]);
		close(fd_file_in);
		close(fd_file_out);

		char *args[] = {argv[3], NULL};

		dup2(fd[0], STDIN_FILENO);
		dup2(fd2[1], STDOUT_FILENO);

		close(fd[0]);
		close(fd2[1]);

		if (!(fd_file_in < 0) && !(fd_file_out < 0))
			execve(argv[3], args, envp);
		write(2, "Deu ruim 2\n", 11);
		exit(2);
	}

	close(fd[0]);
	close(fd[1]);

	pid[3] = fork();

	if (pid[3] == 0)
	{
		// Terceiro comando
		close(fd2[1]);
		close(fd_file_in);

		char *args[] = {argv[4], NULL};

		dup2(fd2[0], STDIN_FILENO);
		dup2(fd_file_out, STDOUT_FILENO);

		close(fd2[0]);
		close(fd_file_out);

		if (!(fd_file_in < 0) && !(fd_file_out < 0))
			execve(argv[4], args, envp);
		write(2, "Deu ruim 3\n", 11);
		exit(2);
	}

	close(fd2[0]);
	close(fd2[1]);

	if (!(fd_file_in < 0))
		close(fd_file_in);
	if (!(fd_file_out < 0))
		close(fd_file_out);

	waitpid(pid[0], NULL, 0);
	waitpid(pid[1], NULL, 0);
	waitpid(pid[2], NULL, 0);

	return (0);
}

// int main(int argc, char *argv[])
// {
// 	int fd = -1;
// 	// if (access(argv[1], F_OK | R_OK) != 0)
// 	// 	perror("Error");
// 	if ((fd = open(argv[1], O_RDONLY)) < 0)
// 		perror("Error description");
// 	else if ((read(fd, 0, 0)) < 0)
// 		perror("Error desc");
// 	else
// 	{
// 		char lido[50];
// 		memset(lido, 0, 50);
//
// 		printf("%d\n", fd);
// 		printf("%s\n", lido);
// 	}
// }
