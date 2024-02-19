/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/18 19:07:51 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/18 22:36:33 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

#define BUFFER_SIZE 4096

static char	*ft_read_all(int fd)
{
	int		chars_readed;
	char	*temp_buffer;
	char	*file_string;
	char	*aux;

	chars_readed = BUFFER_SIZE;
	temp_buffer = (char *) malloc((BUFFER_SIZE + 1));
	file_string = ft_strdup("");
	while (chars_readed == BUFFER_SIZE)
	{
		chars_readed = read(fd, temp_buffer, BUFFER_SIZE);
		temp_buffer[chars_readed] = '\0';
		aux = file_string;
		file_string = ft_strjoin(file_string, temp_buffer);
		free(aux);
	}
	free(temp_buffer);
	return (file_string);
}

int	main(int argc, char *argv[], char *envp[])
{
	int		i;
	int		has_new_line;
	int		has_read;
	int		success;
	size_t	readed;
	size_t	delimiter_size;
	int		pipe_fd[2];
	char	*delimiter;
	char	*line;

	if (argc != 3 || ft_strncmp("here_doc", argv[1], -1))
	{
		write(2, "Usage error.\nExpected: ./pipex here_doc <DELIMITER>\n", 53);
		return (1);
	}
	delimiter = argv[2];
	delimiter_size = ft_strlen(delimiter);
	readed = 1;
	has_new_line = 1;
	has_read = 0;
	line = NULL;
	success = 0;

	pipe(pipe_fd);

	while (1)
	{
		if (readed != 0 && has_new_line == 1)
			ft_putstr_fd("heredoc> ", STDOUT_FILENO);
		line = ft_read_all(STDIN_FILENO);
		readed = ft_strlen(line);
		if (readed == 0 && has_new_line == 1)
		{
			ft_putstr_fd("\nWarning: here-document delimited by end-of-file (wanted '", STDERR_FILENO);
			ft_putstr_fd(delimiter, STDERR_FILENO);
			ft_putstr_fd("')", STDERR_FILENO);
			free(line);
			break ;
		}
		if (ft_strchr(line, '\n') != NULL)
			has_new_line = 1;
		else
			has_new_line = 0;
		if (readed > 0)
			has_read = 1;
		if ((readed - 1) == delimiter_size && ft_strncmp(line, delimiter, delimiter_size) == 0)
		{
			free(line);
			success = 1;
			break ;
		}
		ft_putstr_fd(line, pipe_fd[1]);
		free(line);
		if (has_read == 0)
			break ;
	}

	close(pipe_fd[1]);

	line = ft_read_all(pipe_fd[0]);

	if (success == 0)
		puts("");
	printf("Readed: %s", line);

	free(line);
	close(pipe_fd[0]);

	return (0);
}

// testes

// ./a.out here_doc bomdia
// heredoc> algum texto
// heredoc> bomdia
// Readed: algum texto

// ./a.out here_doc bomdia
// heredoc> algum texto
// heredoc> (CTRL + D)
// Warning: here-document delimited by end-of-file (wanted 'bomdia')
// Readed: algum texto

// ./a.out here_doc bomdia
// heredoc> (CTRL + D)
// Warning: here-document delimited by end-of-file (wanted 'bomdia')
// Readed:

// ./a.out here_doc bomdia
// heredoc> Uma frase
// heredoc> Outra frase
// heredoc> E mais outra
// heredoc> Agora chega
// heredoc> bomdia
// Readed: Uma frase
// Outra frase
// E mais outra
// Agora chega
