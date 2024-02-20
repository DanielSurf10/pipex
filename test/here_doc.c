/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/18 19:07:51 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/20 10:17:43 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int argc, char *argv[], char *envp[])
{
	int		i;
	int		has_new_line;
	int		has_read;
	int		success;
	size_t	readed;
	size_t	delimiter_size;
	int		fd_file_out;
	int		pipe_fd[2];
	char	*delimiter;
	char	*line;

	if (argc != 5 || ft_strncmp("here_doc", argv[1], -1))
	{
		write(2, "Usage error.\nExpected: ./pipex here_doc <DELIMITER> <file_out>\n", 64);
		return (1);
	}
	delimiter = argv[2];
	delimiter_size = ft_strlen(delimiter);
	readed = 1;
	has_new_line = 1;
	has_read = 0;
	line = NULL;
	success = 0;

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);

	if (fd_file_out < 0)
	{
		perror("Invalid output file");
		return(1);
	}

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
