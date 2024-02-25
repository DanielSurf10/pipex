/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/24 22:57:16 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int argc, char *argv[], char *envp[])
{
	t_pipex	command;

	if (argc != 5)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd1> <cmd2> <file_out>\n", 67);
		return (1);
	}

	// Start: Init
	// init files
	command.fd_file_in = open(argv[1], O_RDONLY);
	command.fd_file_out = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (command.fd_file_in < 0)
		perror("Invalid input file");

	if (command.fd_file_out < 0)
		perror("Invalid output file");

	if (access(argv[1], F_OK) != 0 || command.fd_file_out < 0)
		return(1);

	if (command.fd_file_in < 0)		// Se o arquivo existir e não tiver pemissão
		return(0);

	// init: outras coisas
	command.path = get_path_variables(envp);
	command.envp = envp;

	pipe(command.fd_pipe);
	// End: Init

	// Start: Executions
	// Primeiro comando
	command.pid[0] = fork();

	if (command.pid[0] == 0)
	{
		command.command = argv[2];
		command.type = FIRST;
		// printf("%s\n", command.command);
		return (exec_proc(command));
	}


	// Segundo comando
	command.pid[1] = fork();

	if (command.pid[1] == 0)
	{
		command.command = argv[3];
		command.type = LAST;
		// printf("%s\n", command.command);
		return (exec_proc(command));
	}
	// End: Executions


	// Start: Finish
	// close fd's
	close(command.fd_file_in);
	close(command.fd_file_out);
	close_pipe(command.fd_pipe);

	// free's
	free(command.path.home);
	free(command.path.pwd);
	ft_free_split(command.path.path);

	// waits
	waitpid(command.pid[0], NULL, 0);
	waitpid(command.pid[1], &command.return_code, 0);
	// End: Finish

	return ((command.return_code >> 8) & 0xFF);
}
