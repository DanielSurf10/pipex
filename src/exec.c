/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:06:23 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/28 00:16:52 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	dup2_and_close(t_pipex command)
{
	if (command.type == FIRST)
		set_dup2(command.fd_file_in, command.fd_pipe[WRITE]);
	else
		set_dup2(command.fd_pipe[READ], command.fd_file_out);
	close(command.fd_file_in);
	close(command.fd_file_out);
	close_pipe(command.fd_pipe);
}

int	error_manag(t_pipex command, char *command_absolute)
{
	int	return_code;

	if (!command_absolute || access(command_absolute, F_OK) != 0)
	{
		ft_putendl_fd(": command not found", 2);
		return_code = 127;
	}
	else if (access(command_absolute, X_OK) != 0)
	{
		ft_putstr_fd(": Permission denied", 2);
		return_code = 126;
	}
	else
		return_code = 1;
	return (return_code);
}

void	free_all(t_pipex command, char	*command_absolute, char	**command_split)
{
	if (command_absolute)
		free(command_absolute);
	free(command.path.pwd);
	free(command.path.home);
	ft_free_split(command_split);
	ft_free_split(command.path.path);
}

int	exec_command(t_pipex command)
{
	int		return_code;
	char	*command_absolute;
	char	**command_split;

	command_split = ft_split(command.command, ' ');
	command_absolute = get_absolute_path(command_split[0], command.path);
	dup2_and_close(command);
	if (command_absolute && access(command_absolute, F_OK | X_OK) == 0)
		execve(command_absolute, command_split, command.envp);
	ft_putstr_fd(command_split[0], 2);
	return_code = error_manag(command, command_absolute);
	free_all(command, command_absolute, command_split);
	return (return_code);
}
