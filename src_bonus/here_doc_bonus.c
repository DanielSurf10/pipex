/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 18:28:35 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/11 15:34:51 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static char	*read_line(int fd)
{
	int		already_read;
	char	*temp_buffer;
	char	*file_string;
	char	*aux;

	already_read = 0;
	file_string = ft_strdup("");
	while (ft_strchr(file_string, '\n') == NULL)
	{
		temp_buffer = get_next_line(fd);
		if ((ft_strlen(temp_buffer) == 0 && already_read == 0))
			break ;
		if (ft_strlen(temp_buffer) != 0)
			already_read = 1;
		aux = file_string;
		file_string = ft_strjoin(file_string, temp_buffer);
		free(temp_buffer);
		free(aux);
	}
	return (file_string);
}

static void	error_message(int readed, char *delimiter)
{
	if (readed == 0)
	{
		ft_putstr_fd("\nWarning: here-document delimited by end-of-file \
(wanted '",
			STDERR_FILENO);
		ft_putstr_fd(delimiter, STDERR_FILENO);
		ft_putstr_fd("')\n", STDERR_FILENO);
	}
}

int	get_from_here_doc(char *delimiter)
{
	int		readed;
	int		delimiter_size;
	int		pipe_fd[2];
	char	*line;

	delimiter_size = ft_strlen(delimiter);
	pipe(pipe_fd);
	while (1)
	{
		ft_putstr_fd("heredoc> ", STDOUT_FILENO);
		line = read_line(STDIN_FILENO);
		readed = ft_strlen(line);
		if (readed == 0 || ((readed - 1) == delimiter_size
				&& ft_strncmp(line, delimiter, delimiter_size) == 0))
		{
			error_message(readed, delimiter);
			break ;
		}
		ft_putstr_fd(line, pipe_fd[WRITE_FD]);
		free(line);
	}
	free(line);
	get_next_line(-1);
	close(pipe_fd[WRITE_FD]);
	return (pipe_fd[READ_FD]);
}
