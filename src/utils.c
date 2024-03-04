/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 12:46:41 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/04 20:22:13 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	close_pipe(int *fd_pipe)
{
	close(fd_pipe[0]);
	close(fd_pipe[1]);
}

void	set_dup2(int fd_in, int fd_out)
{
	dup2(fd_in, STDIN_FILENO);
	dup2(fd_out, STDOUT_FILENO);
}

void	free_all(t_command command)
{
	free(command.pid);
	free(command.fd_pipes);
	free(command.path.home);
	ft_free_split(command.path.path);
}
