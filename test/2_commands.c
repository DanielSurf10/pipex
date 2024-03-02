/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_commands.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/02 19:25:56 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>

# include "libft.h"

typedef struct s_path
{
	char	*home;
	char	**path;
}	t_path;

typedef struct s_pipex
{
	int		fd_file_in;
	int		fd_file_out;
	int		fd_pipe[2];
	int		pid[2];
	int		type;
	int		return_code;
	char	*command;
	char	**argv;
	char	**envp;
	t_path	path;
}	t_pipex;

//	Passos da execução
//
//	1 - Verifica se tem os 4 argumentos
//
//	2 - Abre um fd para o arquivo de entrada como O_RDONLY
//		2.1 - Verifica se é um fd válido
//		2.1 - Se fd for inválido é escrito uma mensagem de erro no STDERR_FILENO
//
//	3 - Abre um fd para o arquivo de saída como O_WRONLY | O_CREAT | O_TRUNC, 0644
//		3.1 - O_WRONLY - O fd é apenas escrita
//		3.2 - O_CREAT  - Cria o arquivo se não existir
//		3.3 - O_TRUNC  - Apaga todo o conteúdo do arquivo se existir
//		3.4 - Muda a permissão do arquivo para 644 - rw-rw-r--
//			3.4.1 - S_IRUSR - Usuário tem permissão de leitura
//			3.4.2 - S_IWUSR - Usuário tem permissão de escrita
//			3.4.3 - S_IRGRP - Grupo tem permissão de leitura
//			3.4.4 - S_IROTH - Outros tem permissão de leitura
//		3.5 - Se o arquivo já existir ele muda a permissão do arquivo também, se possível
//		3.6 - Se fd for inválido é escrito uma mensagem de erro no STDERR_FILENO
//
//	4 - Cria o pipe
//
//	5 - Faz o primeiro fork e no processo filho
//			Se o fd fd_file_in for válido
//				Pegar caminho do comando
//					Se for um comando como "ls"
//						É necessário pegar o caminho absoluto dele
//					Se for um comando como "./a.out" ou "pipex/a.out"
//						Esse passo não é necessário
//				Faz um dup2 com fd_file_in e STDIN_FILENO
//				Faz um dup2 com fd[1] e STDOUT_FILENO
//				Fecha todos os fd's
//					fd[0] e fd[1] - pipe
//					fd_file_in
//					fd_file_out
//				Verifica se o primeiro comando existe e tem permissão de execução
//					Verifica se o caminho argv[2] existe e tem permissão de execução - access(args[0], F_OK | X_OK) == 0)
//				Executa o arquivo com execve
//					Acaba por aqui o processo
//					A menos que ocorra um problema na execução
//				Se houver algum erro, o erro é escrito no fd STDERR_FILENO e retorna
//
//	7 - Cria o segundo pipe
//
//	8 - Faz o segundo fork

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

char	*get_from_path(char *cmd, t_path path)
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

char	*get_absolute_path(char *cmd, t_path path)
{
	char	*new_command;

	new_command = NULL;
	// if (cmd[0] != '~' && ft_strncmp(cmd, ".", -1) != 0 && ft_strchr(cmd, '/') == NULL)
	if (ft_strchr(cmd, '/') == NULL)
		new_command = get_from_path(cmd, path);
	else
	{
		if (cmd[0] == '~' && path.home != NULL)
			new_command = join_paths(path.home, cmd + 1);
		else
			new_command = ft_strdup(cmd);
	}
	return (new_command);
}

void	exec_process(t_pipex command)
{
	int		return_code;
	char	*cmd_expanded;
	char	**cmd_args;

	if (command.fd_file_in != -1)
	{
		cmd_args = ft_split(command.command, ' ');
		cmd_expanded = get_absolute_path(cmd_args[0], command.path);

		dup2(command.fd_file_in, STDIN_FILENO);
		dup2(command.fd_pipe[1], STDOUT_FILENO);

		close(command.fd_file_in);
		close(command.fd_file_out);

		close(command.fd_pipe[0]);
		close(command.fd_pipe[1]);

		if (access(cmd_args[0], F_OK | X_OK) == 0)
			execve(cmd_args[0], cmd_args, command.envp);

		ft_free_split(cmd_args);

		perror(cmd_args[0]);

		if (access(cmd_args[0], F_OK) != 0)
			exit(127);
		else if (access(cmd_args[0], X_OK))
			exit(126);
	}
	else
	{
		close(command.fd_file_in);
		close(command.fd_file_out);

		close(command.fd_pipe[0]);
		close(command.fd_pipe[1]);
	}
	exit(1);
}

int main(int argc, char *argv[], char *envp[])
{
	// int		fd_file_in;
	// int		fd_file_out;
	// int		fd_pipe[2];
	// int		pid[2];
	// int		error;
	t_pipex	command;
	char	**args;

	if (argc != 5)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <file_out>\n", 64);
		return (1);
	}

	command.path = get_path_variables(envp);

	command.fd_file_in = open(argv[1], O_RDONLY);
	if (command.fd_file_in < 0)
		perror("Invalid input file");

	command.fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (command.fd_file_out < 0)
		perror("Invalid output file");

	// Aqui dá leak de fd do fd_file_out, se der ruim, quando o arquivo de entrada não existir
	// if (access(argv[1], F_OK) != 0 || fd_file_out < 0)
	// 	return(1);

	// if (fd_file_in < 0)		// Se o arquivo existir e não tiver pemissão
	// 	return(0);

	command.envp = envp;
	pipe(command.fd_pipe);
	command.pid[0] = fork();

	if (command.pid[0] == 0)
	{
		// Processo filho primeiro comando
		args = ft_split(argv[2], ' ');

		if (command.fd_file_in != -1)
		{
			dup2(command.fd_file_in, STDIN_FILENO);
			dup2(command.fd_pipe[1], STDOUT_FILENO);

			close(command.fd_file_in);
			close(command.fd_file_out);

			close(command.fd_pipe[0]);
			close(command.fd_pipe[1]);

			if (access(args[0], F_OK | X_OK) == 0)
				execve(args[0], args, envp);

			ft_free_split(args);

			// write(2, "Deu ruim 1\n", 11);
			perror(args[0]);

			if (access(args[0], F_OK) != 0)
				exit(127);
			else if (access(args[0], X_OK))
				exit(126);
		}
		else
		{
			close(command.fd_file_in);
			close(command.fd_file_out);

			close(command.fd_pipe[0]);
			close(command.fd_pipe[1]);
		}
		exit(1);
	}

	command.pid[1] = fork();

	if (command.pid[1] == 0)
	{
		// Processo filho segundo comando
		args = ft_split(argv[3], ' ');

		if (command.fd_file_in != -1)
		{
			dup2(command.fd_pipe[0], STDIN_FILENO);
			dup2(command.fd_file_out, STDOUT_FILENO);

			close(command.fd_file_in);
			close(command.fd_file_out);

			close(command.fd_pipe[0]);
			close(command.fd_pipe[1]);

			if (access(args[0], F_OK | X_OK) == 0)
				execve(args[0], args, envp);

			ft_free_split(args);

			// write(2, "Deu ruim 1\n", 11);
			perror(args[0]);

			if (access(args[0], F_OK) != 0)
				exit(127);
			else if (access(args[0], X_OK))
				exit(126);
		}
		else
		{
			close(command.fd_file_in);
			close(command.fd_file_out);

			close(command.fd_pipe[0]);
			close(command.fd_pipe[1]);
		}
		exit(1);
	}

	waitpid(command.pid[0], NULL, 0);

	close(command.fd_pipe[0]);
	close(command.fd_pipe[1]);
	close(command.fd_file_in);
	close(command.fd_file_out);

	waitpid(command.pid[1], &command.return_code, 0);

	return ((command.return_code >> 8) & 0xFF);
}

// ./pipex /proc/$$/fd/0 "/bin/cat -e" "/bin/grep a" /proc/$$/fd/1
