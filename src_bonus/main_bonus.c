/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/05 19:48:09 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static void	init(t_command *command, int argc, char *argv[], char *envp[])
{
	command->i = 1;
	if (command->fd_file_in < 0)
		perror("Invalid input file");
	command->fd_file_out = open(argv[argc - 1],
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (command->fd_file_out < 0)
		perror("Invalid output file");
	command->path = get_path_variables(envp);
	command->num_cmds = argc - 3;
	command->argv = argv;
	command->envp = envp;
	command->pid = malloc(sizeof(int) * (command->num_cmds));
	command->pipes = malloc(sizeof(t_pipe) * (command->num_cmds - 1));
}

void	exec_commands(t_command *command, int relative_command)
{
	pipe(command->pipes[0].fd_pipe);
	command->pid[0] = fork();
	if (command->pid[0] == 0)
		exec_process(*command, FIRST, 0, relative_command);
	while (command->i < command->num_cmds - 1)
	{
		pipe(command->pipes[command->i].fd_pipe);
		command->pid[command->i] = fork();
		if (command->pid[command->i] == 0)
			exec_process(*command, MID, command->i, relative_command);
		command->i++;
	}
	command->pid[command->num_cmds - 1] = fork();
	if (command->pid[command->num_cmds - 1] == 0)
		exec_process(*command, LAST, command->num_cmds - 1, relative_command);
}

int	main(int argc, char *argv[], char *envp[])
{
	int			return_code;
	int			relative_command;
	t_command	command;

	if (argc < 5 || (ft_strncmp(argv[1], "here_doc", -1) == 0 && argc < 6))
	{
		write(2, "Usage error.\n", 14);
		write(2, "Expected: ./pipex <file_in> <cmd1> <cmd2> ... <cmdn> <file_out>\n", 65);
		write(2, "Expected: ./pipex here_doc <delimiter> <cmd1> <cmd2> ... <cmdn> <file_out>\n", 76);
		return (1);
	}
	if (ft_strncmp(argv[1], "here_doc", -1) == 0)
	{
		relative_command = 3;
		command.fd_file_in = get_from_here_doc(argv[2]);
	}
	else
	{
		relative_command = 2;
		command.fd_file_in = open(argv[1], O_RDONLY);
	}
	init(&command, argc, argv, envp);
	exec_commands(&command, relative_command);
	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);
	command.i = 0;
	while (command.i < command.num_cmds - 1)
	{
		waitpid(command.pid[command.i], NULL, 0);
		close_pipe(command.pipes[command.i]);
		command.i++;
	}
	waitpid(command.pid[command.i], &return_code, 0);
	free_all(command);
	return ((return_code >> 8) & 0xFF);
}
