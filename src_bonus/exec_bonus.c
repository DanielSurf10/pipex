/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:06:23 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/06 12:52:22 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static void	close_pipes(t_command command, int cmd_num)
{
	int	i;

	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);
	i = 0;
	if (cmd_num == 0)
		close_pipe(command.pipes[0]);
	else if (command.num_cmds - 1 == cmd_num)
		while (i < cmd_num)
			close_pipe(command.pipes[i++]);
	else
		while (i <= cmd_num)
			close_pipe(command.pipes[i++]);
}

static void	change_input_and_output(t_command command, int type, int cmd_num)
{
	if (type == FIRST)
		set_dup2(command.fd_file_in, command.pipes[cmd_num].fd_pipe[WRITE]);
	else if (type == MID)
		set_dup2(command.pipes[cmd_num - 1].fd_pipe[READ],
			command.pipes[cmd_num].fd_pipe[WRITE]);
	else if (type == LAST)
		set_dup2(command.pipes[cmd_num - 1].fd_pipe[READ], command.fd_file_out);
}

static void	error_message(char *cmd, int *return_code)
{
	if (!cmd || access(cmd, F_OK) != 0)
	{
		ft_putendl_fd(": Command not found", 2);
		*return_code = 127;
	}
	else if (access(cmd, X_OK) != 0)
	{
		ft_putendl_fd(": Permission denied", 2);
		*return_code = 126;
	}
}

void	exec_process(t_command command, int type, int cmd_num, int relative)
{
	int		return_code;
	char	*cmd;
	char	**args;

	return_code = 1;
	if (type == MID || (type == FIRST && command.fd_file_in != -1)
		|| (type == LAST && command.fd_file_out != -1))
	{
		args = ft_split(command.argv[relative], ' ');
		cmd = expand_path(args[0], command.path);
		change_input_and_output(command, type, cmd_num);
		close_pipes(command, cmd_num);
		if (cmd && access(cmd, F_OK | X_OK) == 0)
			execve(cmd, args, command.envp);
		ft_putstr_fd(args[0], 2);
		error_message(cmd, &return_code);
		free(cmd);
		ft_free_split(args);
	}
	else
		close_pipes(command, cmd_num);
	free_all(command);
	exit(return_code);
}
