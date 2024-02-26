/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/18 19:07:51 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/26 15:25:27 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*read_line(int fd)
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

int	main(int argc, char *argv[], char *envp[])
{
	size_t	readed;
	size_t	delimiter_size;
	int		fd_file_out;
	int		pipe_fd[2];
	char	*delimiter;
	char	*line;

	if (argc != 4 || ft_strncmp("here_doc", argv[1], -1))
	{
		write(2, "Usage error.\nExpected: ./pipex here_doc <DELIMITER> <file_out>\n", 64);
		return (1);
	}
	delimiter = argv[2];
	delimiter_size = ft_strlen(delimiter);
	readed = 1;
	line = NULL;

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);

	if (fd_file_out < 0)
	{
		perror("Invalid output file");
		return(1);
	}

	pipe(pipe_fd);

	while (1)
	{
		ft_putstr_fd("heredoc> ", STDOUT_FILENO);
		line = read_line(STDIN_FILENO);
		readed = ft_strlen(line);
		if (readed == 0)
		{
			ft_putstr_fd("\nWarning: here-document delimited by end-of-file (wanted '", STDERR_FILENO);
			ft_putstr_fd(delimiter, STDERR_FILENO);
			ft_putstr_fd("')\n", STDERR_FILENO);
			free(line);
			break ;
		}
		if ((readed - 1) == delimiter_size && ft_strncmp(line, delimiter, delimiter_size) == 0)
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipe_fd[1]);
		free(line);
	}

	close(pipe_fd[1]);

	line = ft_read_all(pipe_fd[0]);

	ft_putstr_fd(line, fd_file_out);

	free(line);
	close(pipe_fd[0]);
	close(fd_file_out);

	return (0);
}

// testes

// ./a.out here_doc bomdia
// heredoc> algum texto
// heredoc> bomdia
//
// algum texto

// ./a.out here_doc bomdia
// heredoc> algum texto
// heredoc> (CTRL + D)
// Warning: here-document delimited by end-of-file (wanted 'bomdia')
//
// algum texto

// ./a.out here_doc bomdia
// heredoc> (CTRL + D)
// Warning: here-document delimited by end-of-file (wanted 'bomdia')
//
//

// ./a.out here_doc bomdia
// heredoc> Uma frase
// heredoc> Outra frase
// heredoc> E mais outra
// heredoc> Agora chega
// heredoc> bomdia
//
// Uma frase
// Outra frase
// E mais outra
// Agora chega
