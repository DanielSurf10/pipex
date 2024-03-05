/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   any_commands.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 12:56:26 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/05 00:36:14 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// enum e_process
// {
// 	FIRST = 0,
// 	MID,
// 	LAST
// };
//
// typedef struct s_path
// {
// 	char	*home;
// 	// char	*pwd;
// 	char	**path;
// }	t_path;
//
// typedef struct s_command
// {
// 	int		fd_file_in;
// 	int		fd_file_out;
// 	int		num_cmds;
// 	int		*fd_pipes;
// 	int		*pid;
// 	char	**argv;
// 	char	**envp;
// 	t_path	path;
// }	t_command;

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

// Primeiro comando:
//	- fd_file_in				- usado
//	- fd_file_out				- não usado
//	- pipe de entrada - leitura	- não usado, não deve ser fechado (ele não existe)
//	- pipe de saída   - escrita	- usado
//		- fd[0] (leitura) não usado
//		- fd[1] (escrita) usado
//
// Comandos do meio:
//	- fd_file_in				- não usado
//	- fd_file_out				- não usado
//	- pipe de entrada - leitura - usado
//		- fd[0] (leitura) usado
//		- fd[1] (escrita) não usado
//	- pipe de saída   - escrita	- usado
//		- fd[0] (leitura) não usado
//		- fd[1] (escrita) usado
//
// Último comando:
//	- fd_file_in				- não usado
//	- fd_file_out				- usado
//	- pipe de entrada - leitura - usado
//		- fd[0] (leitura) usado
//		- fd[1] (escrita) não usado
//	- pipe para saída - escrita	- não usado, não deve ser fechado (ele não existe)

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

// int	exec_proc(t_command command, t_path path)
// {
// 	int		return_code;
// 	char	*command_absolute;
// 	char	**command_split;
//
// 	command_split = ft_split(command.command, ' ');
	// printf("%s\n", command_split[0]);
// 	command_absolute = get_command_from_path(command_split[0], path);
//
// 	// Se for o primeiro ele deve:
// 	//	- Fazer o dup2 com command.fd_file_in e STDIN_FILENO
// 	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
// 	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
// 	//	- Fechar o fd do arquivo de saída - command.fd_file_out
// 	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
// 	//	- Fechar a escrita do pipe de saída - command.fd_pipe_out[WRITE]
// 	if (command.type == FIRST)
// 	{
// 		dup2(command.fd_file_in, STDIN_FILENO);
// 		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);
//
// 		close(command.fd_file_in);
// 		close(command.fd_file_out);
// 		close_pipe(command.fd_pipe_out);
// 	}
//
// 	// Se for algum comando do meio ele deve:
// 	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
// 	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
// 	//	- Fechar os fd's dos 2 arquivos, entrada e saída
// 	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
// 	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[WRITE]
// 	//	- Fechar a leitura do pipe de saída - command.fd_pipe_out[READ]
// 	//	- Fechar a escrita do pipe de saíða - command.fd_pipe_out[WRITE]
// 	else if (command.type == MID)
// 	{
// 		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
// 		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);
//
// 		close(command.fd_file_in);
// 		close(command.fd_file_out);
// 		close_pipe(command.fd_pipe_in);
// 		close_pipe(command.fd_pipe_out);
// 	}
//
// 	// Se for o último comando ele deve:
// 	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
// 	//	- Fazer o dup2 com command.fd_file_out e STDOUT_FILENO
// 	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
// 	//	- Fechar o fd do arquivo de saída - command.fd_file_out
// 	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
// 	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[WRITE]
// 	else
// 	{
// 		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
// 		dup2(command.fd_file_out, STDOUT_FILENO);
//
// 		close(command.fd_file_in);
// 		close(command.fd_file_out);
// 		close_pipe(command.fd_pipe_in);
// 	}
//
// 	if (command_absolute && access(command_absolute, F_OK | X_OK) == 0)
// 		execve(command_absolute, command_split, command.envp);
//
// 	// write(2, "Deu ruim 1\n", 11);
// 	perror(command_absolute);
//
// 	if (!command_absolute || access(command_absolute, F_OK) != 0)
// 		return_code = 127;
// 	else if (access(command_absolute, X_OK) != 0)
// 		return_code = 126;
// 	else
// 		return_code = 1;
//
// 	ft_free_split(command_split);
// 	free(command_absolute);
// 	free(path.home);
// 	free(path.pwd);
// 	ft_free_split(path.path);
// 	return (return_code);
// }
//
//	Abre os fd's dos arquivos
//	Verifica se deu erro
//
//	faz um malloc de um array de int -> pid
//	- Com o tamanho do número de comandos a serem executados
//	- Esse array vai armazenar todos os pid's
//
//	abre o primeiro pipe e põe em pipe_out
//	faz o primeiro fork e salva o pid no array -> pid[0]
//	Dentro do child
//		dá free no array de pid's -> free(pid)
//		faz o dup2 do fd_file_in e STDIN
//		faz o dup2 do pipe_out[1] e STDOUT
//		fecha todos os fd, exceto STDIN e STDOUT
//		executa o argv[2] -> primeiro comando
//	i = 3
//
//	while (i < argc - 2)
//		fecha os fd do pipe pipe_in
//		pipe_out vai para pipe_in
//		abre um pipe no pipe_out
//		faz um fork e põe o pid no array de pi's -> pid[i - 3]
//		Dentro do child:
//			dá free no array de pid's -> free(pid)
//			faz o dup2 do pipe_in[0] e STDIN
//			faz o dup2 do pipe_out[1] e STDOUT
//			fecha todos os fd, exceto STDIN e STDOUT
//			executa o comando argv[i]
//		i++
//
//	Se tiver mais do que 2 comandos:
//		- foi preciso fazer mais do que 1 pipe
//		- então precisa dar close no pipe_in
//		fecha os fd do pipe pipe_in
//	pipe_out vai para pipe_in
//	faz o último fork e salva o pid no array -> pid[quantidade_de_comandos - 1]
//	Dentro do child
//		dá free no array de pid's -> free(pid)
//		faz o dup2 do pipe_in[0] e STDIN
//		faz o dup2 do fd_file_out e STDOUT
//		fecha todos os fd, exceto STDIN e STDOUT
//		executa o comando argv[argc - 2] -> último comando
//
//	fecha todos os fd's
//		- fd's dos arquivos de entrada e saída
//		- pipe_in
//
//	i = 0
//	while (i < quantidade_de_comandos)
//		waitpid(pid[i], &return_code, 0);
//		i++
//	return ((return_code >> 8) & 0xFF);
//

void	exec_process(t_command command, int type, int cmd_num)
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
		args = ft_split(command.argv[cmd_num + 2], ' ');
		cmd = expand_path(args[0], command.path);

		if (type == FIRST)
			set_dup2(command.fd_file_in, command.fd_pipes[(cmd_num * 2) + 1]);
		else if (type == MID)
			set_dup2(command.fd_pipes[(cmd_num - 1) * 2],
				command.fd_pipes[(cmd_num * 2) + 1]);
		else if (type == LAST)
			set_dup2(command.fd_pipes[(cmd_num - 1) * 2], command.fd_file_out);

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

	while (i < cmd_num * 2 || (cmd_num == 0 && i == 0))
		close_pipe(command.fd_pipes + (i += 2));

	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);

	free(command.pid);
	free(command.fd_pipes);
	free(command.path.home);
	ft_free_split(command.path.path);
	exit(return_code);
}

int	main(int argc, char *argv[], char *envp[])
{
	t_command	command;
	int			i;
	int			return_code;

	if (argc < 5)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd1> <cmd2> ... <cmdn> <file_out>\n", 78);
		return (1);
	}

	command.fd_file_in = open(argv[1], O_RDONLY);

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
	command.fd_pipes = malloc(sizeof(int) * (command.num_cmds - 1) * 2);

	pipe(command.fd_pipes);
	command.pid[0] = fork();

	if (command.pid[0] == 0)
		exec_process(command, FIRST, 0);

	while (i < command.num_cmds - 1)
	{
		pipe(command.fd_pipes + (i * 2));
		command.pid[i] = fork();
		if (command.pid[i] == 0)
			exec_process(command, MID, i);
		i++;
	}

	command.pid[command.num_cmds - 1] = fork();

	if (command.pid[command.num_cmds - 1] == 0)
		exec_process(command, LAST, command.num_cmds - 1);

	if (command.fd_file_in != -1)
		close(command.fd_file_in);
	if (command.fd_file_out != -1)
		close(command.fd_file_out);

	i = 0;

	while (i < (command.num_cmds - 1))
	{
		waitpid(command.pid[i], NULL, 0);
		close_pipe(command.fd_pipes + (i * 2));
		i++;
	}

	waitpid(command.pid[i], &return_code, 0);
	free(command.path.home);
	free(command.pid);
	free(command.fd_pipes);
	ft_free_split(command.path.path);

	return ((return_code >> 8) & 0xFF);
}

// testes:
// ./pipex test/files_test/file.txt "/bin/sleep 5" "/bin/ls" test/files_test/out
//
