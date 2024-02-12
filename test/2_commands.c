/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_commands.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/12 19:43:27 by danbarbo         ###   ########.fr       */
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
	int	pid[2];
	int	error_code;

	if (argc != 5)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <file_out>\n", 64);
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
		char *args[] = {argv[2], NULL};

		close(fd[0]);
		if (!(fd_file_out < 0))
			close(fd_file_out);

		if (!(fd_file_in < 0))
		{
			dup2(fd_file_in, STDIN_FILENO);
			close(fd_file_in);
		}

		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		if (!(fd_file_in < 0) && !(fd_file_out < 0) && access(argv[2], F_OK | X_OK) == 0)
			execve(argv[2], args, envp);

		if (fd_file_in < 0 || fd_file_out < 0)
			error_code = 1;
		else if (access(argv[2], F_OK))
			error_code = 127;
		else
			error_code = 126;

		// write(2, "Deu ruim 1\n", 11);
		perror("Commmand 1");
		exit(error_code);
	}

	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		char *args[] = {argv[3], NULL};

		if (!(fd_file_in < 0))
			close(fd_file_in);
		close(fd[1]);

		if (!(fd_file_out < 0))
		{
			dup2(fd_file_out, STDOUT_FILENO);
			close(fd_file_out);
		}

		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);

		if (!(fd_file_in < 0) && !(fd_file_out < 0) && access(argv[3], F_OK | X_OK) == 0)
			execve(argv[3], args, envp);

		if (!(fd_file_in < 0) && !(fd_file_out < 0))
			error_code = 1;
		else if (access(argv[3], F_OK))
			error_code = 127;
		else
			error_code = 126;

		// write(2, "Deu ruim 2\n", 11);
		perror("Commmand 2");
		exit(error_code);
	}

	close(fd[0]);
	close(fd[1]);

	if (!(fd_file_in < 0))
		close(fd_file_in);
	if (!(fd_file_out < 0))
		close(fd_file_out);

	int	ret[2];

	waitpid(pid[0], &ret[0], 0);
	waitpid(pid[1], &ret[1], 0);

	ret[0] = (ret[0] >> 8) & 0xFF;
	ret[1] = (ret[1] >> 8) & 0xFF;

	if (access(argv[3], F_OK) != 0)
		ret[1] = 127;
	else if (access(argv[3], X_OK) != 0)
		ret[1] = 126;

	return (ret[1]);
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
