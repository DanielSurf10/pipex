/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   any_commands.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 12:56:26 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/20 00:44:00 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

enum e_process
{
	FIRST = 0,
	MID,
	LAST
};

enum e_fd
{
	READ = 0,
	WRITE
};

typedef struct s_command
{
	int		type;
	int		fd_pipe_in[2];
	int		fd_pipe_out[2];
	int		fd_file_in;
	int		fd_file_out;
	char	*command;
	char	**envp;
}	t_command;

// Primeiro comando:
//	- fd_file_in				- usado
//	- fd_file_out				- não usado
//	- pipe de entrada - leitura	- não usado, não deve ser fechado (ele não existe)
//	- pipe de saída   - escrita	- usado
//		- fd[0] (leitura) não usado
//		- fd[1] (escrita) usado

// Comandos do meio:
//	- fd_file_in				- não usado
//	- fd_file_out				- não usado
//	- pipe de entrada - leitura - usado
//		- fd[0] (leitura) usado
//		- fd[1] (escrita) não usado
//	- pipe de saída   - escrita	- usado
//		- fd[0] (leitura) não usado
//		- fd[1] (escrita) usado

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

int	exec_proc(t_command command)
{
	int		return_code;
	char	**command_split;

	command_split = ft_split(command.command, ' ');

	// Se for o primeiro ele deve:
	//	- Fazer o dup2 com command.fd_file_in e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
	//	- Fechar o fd do arquivo de saída - command.fd_file_out
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fechar a escrita do pipe de saída - command.fd_pipe_out[WRITE]
	if (command.type == FIRST)
	{
		dup2(command.fd_file_in, STDIN_FILENO);
		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);

		close(command.fd_file_in);
		close(command.fd_file_out);
		close(command.fd_pipe_out[READ]);
		close(command.fd_pipe_out[WRITE]);
	}

	// Se for algum comando do meio ele deve:
	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
	//	- Fechar os fd's dos 2 arquivos, entrada e saída
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[WRITE]
	//	- Fechar a leitura do pipe de saída - command.fd_pipe_out[READ]
	//	- Fechar a escrita do pipe de saíða - command.fd_pipe_out[WRITE]
	else if (command.type == MID)
	{
		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);

		close(command.fd_file_in);
		close(command.fd_file_out);
		close(command.fd_pipe_in[READ]);
		close(command.fd_pipe_in[WRITE]);
		close(command.fd_pipe_out[READ]);
		close(command.fd_pipe_out[WRITE]);
	}

	// Se for o último comando ele deve:
	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_file_out e STDOUT_FILENO
	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
	//	- Fechar o fd do arquivo de saída - command.fd_file_out
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[WRITE]
	else
	{
		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
		dup2(command.fd_file_out, STDOUT_FILENO);

		close(command.fd_file_in);
		close(command.fd_pipe_in[WRITE]);
		close(command.fd_file_out);
		close(command.fd_pipe_in[READ]);
	}

	if (access(command_split[0], F_OK | X_OK) == 0)
		execve(command_split[0], command_split, command.envp);

	// write(2, "Deu ruim 1\n", 11);
	perror(command_split[0]);

	if (access(command_split[0], F_OK) != 0)
		return_code = 127;
	else if (access(command_split[0], X_OK) != 0)
		return_code = 126;
	else
		return_code = 1;

	ft_free_split(command_split);
	return (return_code);
}

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

int	main(int argc, char *argv[], char *envp[])
{
	// int	fd_file_in;
	// int	fd_file_out;
	// int	fd[2];
	// int	fd2[2];
	// int	pid[3];
	t_command	command;
	int			command_iter;
	int			return_code;
	int			*pid;

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

	if (command.fd_file_in < 0 || command.fd_file_out < 0)
		return(1);

	command_iter = 3;
	command.envp = envp;
	pid = malloc(sizeof(int) * (argc - 3));

	pipe(command.fd_pipe_out);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		free(pid);
		command.command = argv[2];		// Primeiro comando
		command.type = FIRST;
		return (exec_proc(command));
	}

	command.type = MID;
	while (command_iter < argc - 2)
	{
		if (command_iter > 3)
			close_pipe(command.fd_pipe_in);
		ft_memcpy(command.fd_pipe_in, command.fd_pipe_out, sizeof(int) * 2);
		pipe(command.fd_pipe_out);
		pid[command_iter - 2] = fork();

		if (pid[command_iter - 2] == 0)
		{
			free(pid);
			command.command = argv[command_iter];
			return (exec_proc(command));
		}
		command_iter++;
	}

	if (argc - 3 > 2)	// Não tem pipe_in se só tiver 2 comandos
		close_pipe(command.fd_pipe_in);
	ft_memcpy(command.fd_pipe_in, command.fd_pipe_out, sizeof(int) * 2);
	pid[command_iter - 2] = fork();
	command.type = LAST;

	if (pid[command_iter - 2] == 0)
	{
		free(pid);
		command.command = argv[command_iter];
		return (exec_proc(command));
	}

	close_pipe(command.fd_pipe_in);
	close(command.fd_file_in);
	close(command.fd_file_out);

	command_iter = 0;
	while (command_iter < (argc - 3))
	{
		waitpid(pid[command_iter], &return_code, 0);
		command_iter++;
	}
	free(pid);
	return ((return_code >> 8) & 0xFF);
}

// testes:
// ./pipex test/files_test/file.txt "/bin/sleep 5" "/bin/ls" test/files_test/out
