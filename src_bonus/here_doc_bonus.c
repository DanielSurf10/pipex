/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 18:28:35 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/05 20:01:03 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static char	*read_line(int fd)
{
	int		chars_readed;
	int		already_read;
	char	*temp_buffer;
	char	*file_string;
	char	*aux;

	already_read = 0;
	chars_readed = BUFFER_SIZE;
	temp_buffer = (char *) malloc((BUFFER_SIZE + 1));
	file_string = ft_strdup("");
	while (ft_strchr(file_string, '\n') == NULL)
	{
		chars_readed = read(fd, temp_buffer, BUFFER_SIZE);
		temp_buffer[chars_readed] = '\0';
		if ((chars_readed == 0 && already_read == 0))
			break ;
		if (chars_readed != 0)
			already_read = 1;
		aux = file_string;
		file_string = ft_strjoin(file_string, temp_buffer);
		free(aux);
	}
	free(temp_buffer);
	return (file_string);
}

void	print_error_message(char *delimiter)
{
	ft_putstr_fd("\nWarning: here-document delimited by end-of-file (wanted '",
		STDERR_FILENO);
	ft_putstr_fd(delimiter, STDERR_FILENO);
	ft_putstr_fd("')\n", STDERR_FILENO);
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
			if (readed == 0)
				print_error_message(delimiter);
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}
