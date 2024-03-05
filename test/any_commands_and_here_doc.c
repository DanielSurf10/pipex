/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   any_commands_and_here_doc.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 12:56:26 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/05 18:32:05 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>		// Pode usar se for usar strerror
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>

# include "libft.h"

// enums

enum e_fd
{
	READ = 0,
	WRITE
};

enum e_process
{
	FIRST = 0,
	MID,
	LAST
};

// structs

typedef struct s_path
{
	char	*home;
	char	**path;
}	t_path;

typedef struct s_pipe
{
	int	fd_pipe[2];
}	t_pipe;

typedef struct s_command
{
	int		i;
	int		fd_file_in;
	int		fd_file_out;
	int		num_cmds;
	int		*pid;
	t_pipe	*pipes;
	char	**argv;
	char	**envp;
	t_path	path;
}	t_command;

char	**split_path(char *str)
{
	char	*start;
	char	**split;

	start = ft_strchr(str, '=') + 1;
	split = ft_split(start, ':');
	return (split);
}

char	*join_paths(char *absolute, char *relative)
{
	char	total_size;
	int		absolute_size;
	char	*str;

	absolute_size = ft_strlen(absolute);
	if (absolute[absolute_size - 1] == '/')
		absolute_size--;
	if (relative[0] == '/')
		relative++;
	total_size = absolute_size + ft_strlen(relative) + 2;
	str = malloc(total_size);
	ft_strlcpy(str, absolute, total_size);
	str[absolute_size] = '/';
	str[absolute_size + 1] = '\0';
	ft_strlcat(str, relative, total_size);
	return (str);
}

t_path	get_path_variables(char **envp)
{
	int		i;
	t_path	path;

	i = 0;
	ft_bzero(&path, sizeof(t_path));
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
			path.path = split_path(envp[i]);
		else if (ft_strncmp(envp[i], "HOME", 3) == 0)
			path.home = ft_strdup(ft_strchr(envp[i], '=') + 1);
		i++;
	}
	return (path);
}

char	*expand_from_path(char *cmd, t_path path)
{
	int		i;
	char	*new_command;

	i = 0;
	new_command = NULL;
	while (path.path && path.path[i])
	{
		new_command = join_paths(path.path[i], cmd);
		if (access(new_command, F_OK | X_OK) == 0)
			break ;
		free(new_command);
		new_command = NULL;
		i++;
	}
	return (new_command);
}

char	*expand_path(char *cmd, t_path path)
{
	char	*new_command;

	new_command = NULL;
	// if (cmd[0] != '~' && ft_strncmp(cmd, ".", -1) != 0 && ft_strchr(cmd, '/') == NULL)
	if (ft_strchr(cmd, '/') == NULL)
		new_command = expand_from_path(cmd, path);
	else
	{
		if (cmd[0] == '~' && path.home != NULL)
			new_command = join_paths(path.home, cmd + 1);
		else
			new_command = ft_strdup(cmd);
	}
	return (new_command);
}

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

void	exec_process(t_command command, int type, int cmd_num, int relative_command)
{
	int		i;
	int		return_code;
	char	*cmd;
	char	**args;

	i = 0;
	return_code = 1;
	if (type == MID || (type == FIRST && command.fd_file_in != -1)
		|| (type == LAST && command.fd_file_out != -1))
	{
		args = ft_split(command.argv[relative_command], ' ');
		cmd = expand_path(args[0], command.path);

		if (type == FIRST)
			set_dup2(command.fd_file_in, command.pipes[cmd_num].fd_pipe[WRITE]);
		else if (type == MID)
			set_dup2(command.pipes[cmd_num - 1].fd_pipe[READ],
				command.pipes[cmd_num].fd_pipe[WRITE]);
		else if (type == LAST)
			set_dup2(command.pipes[cmd_num - 1].fd_pipe[READ], command.fd_file_out);

		while (i < cmd_num || (cmd_num == 0 && i == 0))
		{
			close_pipe(command.pipes[i]);
			i++;
		}
		if (command.fd_file_in != -1)
			close(command.fd_file_in);
		if (command.fd_file_out != -1)
			close(command.fd_file_out);

		if (cmd && access(cmd, F_OK | X_OK) == 0)
			execve(cmd, args, command.envp);

		ft_putstr_fd(args[0], 2);
		if (!cmd || access(cmd, F_OK) != 0)
		{
			ft_putendl_fd(": Command not found", 2);
			return_code = 127;
		}
		else if (access(cmd, X_OK) != 0)
		{
			ft_putendl_fd(": Permission denied", 2);
			return_code = 126;
		}

		if (!cmd || access(cmd, F_OK) != 0)
			return_code = 127;

		else if (access(cmd, X_OK) != 0)
			return_code = 126;

		free(cmd);
		ft_free_split(args);
	}
	else
		while (i < cmd_num || (cmd_num == 0 && i == 0))
		{
			close_pipe(command.pipes[i]);
			i++;
		}

	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);

	free(command.pid);
	free(command.pipes);
	free(command.path.home);
	ft_free_split(command.path.path);
	exit(return_code);
}

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

int	get_from_here_doc(char *delimiter)
{
	int		readed;
	int		delimiter_size;
	int		pipe_fd[2];
	char	*line;

	delimiter_size = ft_strlen(delimiter);
	readed = 1;
	line = NULL;
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
	return (pipe_fd[0]);
}

int	main(int argc, char *argv[], char *envp[])
{
	int			i;
	int			relative_command;
	int			return_code;
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

	if (command.fd_file_in < 0)
		perror("Invalid input file");

	command.fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (command.fd_file_out < 0)
		perror("Invalid output file");

	command.path = get_path_variables(envp);

	i = 1;
	command.num_cmds = argc - 3;
	command.argv = argv;
	command.envp = envp;
	command.pid = malloc(sizeof(int) * (command.num_cmds));
	command.pipes = malloc(sizeof(t_pipe) * (command.num_cmds - 1));

	pipe(command.pipes[0].fd_pipe);
	command.pid[0] = fork();

	if (command.pid[0] == 0)
		exec_process(command, FIRST, 0, relative_command);

	relative_command++;

	while (i < command.num_cmds - 1)
	{
		pipe(command.pipes[i].fd_pipe);
		command.pid[i] = fork();
		if (command.pid[i] == 0)
			exec_process(command, MID, i, relative_command);
		relative_command++;
		i++;
	}

	command.pid[command.num_cmds - 1] = fork();

	if (command.pid[command.num_cmds - 1] == 0)
		exec_process(command, LAST, command.num_cmds - 1, argc - 2);

	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);

	i = 0;

	while (i < (command.num_cmds - 1))
	{
		waitpid(command.pid[i], NULL, 0);
		close_pipe(command.pipes[i]);
		i++;
	}

	waitpid(command.pid[i], &return_code, 0);
	free(command.path.home);
	free(command.pid);
	free(command.pipes);
	ft_free_split(command.path.path);

	return ((return_code >> 8) & 0xFF);
}

// testes:
// ./pipex test/files_test/file.txt "/bin/sleep 5" "/bin/ls" test/files_test/out
//
