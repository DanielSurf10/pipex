/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 12:46:41 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/05 12:42:52 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	close_pipe(t_pipe pipe_to_close)
{
	close(pipe_to_close.fd_pipe[0]);
	close(pipe_to_close.fd_pipe[1]);
}

void	set_dup2(int fd_in, int fd_out)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(fd_out, STDOUT_FILENO);
}

void	free_all(t_command command)
{
	free(command.pid);
	free(command.pipes);
	free(command.path.home);
	ft_free_split(command.path.path);
}
