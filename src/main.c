/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/28 00:19:28 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	init(t_pipex *command, char *argv[], char *envp[])
{
	command->argv = argv;
	command->envp = envp;
	command->fd_file_in = open(command->argv[1], O_RDONLY);
	command->fd_file_out = open(command->argv[4],
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (command->fd_file_in < 0)
		perror("Invalid input file");
	if (command->fd_file_out < 0)
		perror("Invalid output file");
	if (access(argv[1], F_OK) == 0 && command->fd_file_out != -1
		&& command->fd_file_in != -1)
	{
		pipe(command->fd_pipe);
		command->path = get_path_variables(command->envp);
	}
	else
	{
		if (command->fd_file_in != -1)
			close(command->fd_file_in);
		if (command->fd_file_out != -1)
			close(command->fd_file_out);
	}
}

int	exec_child(t_pipex *command, int type)
{
	command->command = command->argv[2 + type];
	command->type = type;
	return (exec_command(*command));
}

void	free_and_close(t_pipex *command)
{
	close(command->fd_file_in);
	close(command->fd_file_out);
	close_pipe(command->fd_pipe);
	free(command->path.home);
	free(command->path.pwd);
	ft_free_split(command->path.path);
}

int	main(int argc, char *argv[], char *envp[])
{
	int		i;
	t_pipex	command;

	if (argc != 5)
	{
		write(2, "Usage error.\n", 14);
		write(2, "Expected: ./pipex <file_in> <cmd1> <cmd2> <file_out>\n", 54);
		return (1);
	}
	init(&command, argv, envp);
	if (access(argv[1], F_OK) != 0 || command.fd_file_out < 0)
		return (1);
	if (command.fd_file_in < 0)			// Algo aqui parece estar errado
		return (0);
	i = -1;
	while (++i < 2)
	{
		command.pid[i] = fork();
		if (command.pid[i] == 0)
			return (exec_child(&command, i));
	}
	waitpid(command.pid[0], NULL, 0);
	free_and_close(&command);							// Precisa fechar em baixo para
	waitpid(command.pid[1], &command.return_code, 0);	// evitar broken pipe e no meio
	return ((command.return_code >> 8) & 0xFF);			// para não fazer o 2º child esperar
}
