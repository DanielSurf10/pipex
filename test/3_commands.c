/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_commands.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/13 16:46:04 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main(int argc, char *argv[], char *envp[])
{
	int	fd_file_in;
	int	fd_file_out;
	int	fd[2];
	int	fd2[2];
	int	pid[3];
	int	error;

	if (argc != 6)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <cmd> <file_out>\n", 71);
		return (1);
	}

	fd_file_in = open(argv[1], O_RDONLY);

	if (fd_file_in < 0)
		perror("Invalid input file");

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd_file_out < 0)
		perror("Invalid output file");

	if (fd_file_in < 0 || fd_file_out < 0)
		exit(1);

	pipe(fd);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		// Processo filho primeiro comando
		char **args = ft_split(argv[2], ' ');

		close(fd[0]);
		close(fd_file_out);

		dup2(fd_file_in, STDIN_FILENO);
		close(fd_file_in);

		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		if (access(args[0], F_OK | X_OK) == 0)
			execve(args[0], args, envp);

		ft_free_split(args);

		// write(2, "Deu ruim 1\n", 11);
		perror("Commmand 1");

		if (access(args[0], F_OK | X_OK) != 0)
			return (127);
		else if (errno == EACCES)
			return (126);
		else
			return (1);
	}

	pipe(fd2);
	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		char **args = ft_split(argv[3], ' ');

		close(fd[1]);
		close(fd2[0]);

		close(fd_file_in);
		close(fd_file_out);

		dup2(fd[0], STDIN_FILENO);
		dup2(fd2[1], STDOUT_FILENO);

		close(fd[0]);
		close(fd2[1]);

		if (access(args[0], F_OK | X_OK) == 0)
			execve(args[0], args, envp);

		ft_free_split(args);

		// write(2, "Deu ruim 2\n", 11);
		perror("Commmand 2");

		if (access(args[0], F_OK | X_OK) != 0)
			return (127);
		else if (errno == EACCES)
			return (126);
		else
			return (1);
	}

	close(fd[0]);
	close(fd[1]);

	pid[2] = fork();

	if (pid[2] == 0)
	{
		// Terceiro comando
		char **args = ft_split(argv[4], ' ');

		close(fd2[1]);
		close(fd_file_in);

		dup2(fd2[0], STDIN_FILENO);
		close(fd2[0]);

		dup2(fd_file_out, STDOUT_FILENO);
		close(fd_file_out);

		if (access(args[0], F_OK | X_OK) == 0)
			execve(args[0], args, envp);

		ft_free_split(args);

		// write(2, "Deu ruim 3\n", 11);
		perror("Commmand 3");

		if (access(args[0], F_OK | X_OK) != 0)
			return (127);
		else if (errno == EACCES)
			return (126);
		else
			return (1);
	}

	close(fd2[0]);
	close(fd2[1]);

	close(fd_file_in);
	close(fd_file_out);

	waitpid(pid[0], NULL, 0);
	waitpid(pid[1], NULL, 0);
	waitpid(pid[2], &error, 0);

	return ((error >> 8) & 0xFF);
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
