/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   any_commands.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 12:56:26 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/19 19:48:02 by danbarbo         ###   ########.fr       */
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

int	exec_proc(t_command command)
{
	char **command_split;

	command_split = ft_split(command.command, ' ');

	// Se for o primeiro ele deve:
	//	- Fechar o fd do arquivo de saída - command.fd_file_out
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fazer o dup2 com command.fd_file_in e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
	//	- Fechar a escrita do pipe de saída - command.fd_pipe_out[WRITE]
	if (command.type == FIRST)
	{
		close(command.fd_file_out);
		close(command.fd_pipe_out[READ]);

		dup2(command.fd_file_in, STDIN_FILENO);
		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);

		close(command.fd_file_in);
		close(command.fd_pipe_out[WRITE]);
	}

	// Se for algum comando do meio ele deve:
	//	- Fechar os fd's dos 2 arquivos, entrada e saída
	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fechar a escrita do pipe de saída - command.fd_pipe_out[WRITE]
	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_pipe_out[WRITE] e STDOUT_FILENO
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fechar a escrita do pipe de saíða - command.fd_pipe_out[WRITE]
	else if (command.type == MID)
	{
		close(command.fd_file_in);
		close(command.fd_file_out);
		close(command.fd_pipe_in[WRITE]);
		close(command.fd_pipe_out[READ]);

		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
		dup2(command.fd_pipe_out[WRITE], STDOUT_FILENO);

		close(command.fd_pipe_in[READ]);
		close(command.fd_pipe_out[WRITE]);
	}

	// Se for o último comando ele deve:
	//	- Fechar o fd do arquivo de entrada - command.fd_file_in
	//	- Fechar a escrita do pipe de entrada - command.fd_pipe_in[READ]
	//	- Fazer o dup2 com command.fd_pipe_in[READ] e STDIN_FILENO
	//	- Fazer o dup2 com command.fd_file_out e STDOUT_FILENO
	//	- Fechar o fd do arquivo de saída - command.fd_file_out
	//	- Fechar a leitura do pipe de entrada - command.fd_pipe_in[READ]
	else
	{
		close(command.fd_file_in);
		close(command.fd_pipe_in[WRITE]);

		dup2(command.fd_pipe_in[READ], STDIN_FILENO);
		dup2(command.fd_file_out, STDOUT_FILENO);

		close(command.fd_file_out);
		close(command.fd_pipe_in[READ]);
	}

	if (access(command_split[0], F_OK | X_OK) == 0)
		execve(command_split[0], command_split, command.envp);

	ft_free_split(command_split);

	// write(2, "Deu ruim 1\n", 11);
	perror("Commmand 1");

	if (access(command_split[0], F_OK) != 0)
		return (127);
	else if (access(command_split[0], X_OK) != 0)
		return (126);
	else
		return (1);
}

// Abre os fd's dos arquivos
// Verifica se deu erro
// faz um malloc de um array de int
//	- Quantidade: número de comandos
//	- Vai armazenar todos os pid's
// abre o primeiro pipe pipe_in
// faz o primeiro fork array pid[0]
// executa o argv[2]
// i = 3
//
// while
// fecha os fd do pipe pipe_in
// pipe_in vai para pipe_out
// abre um pipe no pipe_in
// faz um fork e põe o pid no array de pi's
// executa comando arg[i]
// i++ -> Isso até (i < argc - 1)
//

int	main(int argc, char *argv[], char *envp[])
{
	int	fd_file_in;
	int	fd_file_out;
	int	fd[2];
	int	fd2[2];
	int	pid[3];
	int	error;

	if (argc != 6)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <cmd> <file_out>\n", 71);
		return (1);
	}

	fd_file_in = open(argv[1], O_RDONLY);

	if (fd_file_in < 0)
		perror("Invalid input file");

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd_file_out < 0)
		perror("Invalid output file");

	if (fd_file_in < 0 || fd_file_out < 0)
		exit(1);

	pipe(fd);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		// Processo filho primeiro comando
		t_command	command;

		command.command = argv[2];
		command.type = FIRST;
		command.fd_pipe_out[0] = fd[0];
		command.fd_pipe_out[1] = fd[1];
		command.envp = envp;
		command.fd_file_in = fd_file_in;
		command.fd_file_out = fd_file_out;

		return (exec_proc(command));
	}

	pipe(fd2);
	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		t_command	command;

		command.command = argv[3];
		command.type = MID;
		command.fd_pipe_in[0] = fd[0];
		command.fd_pipe_in[1] = fd[1];
		command.fd_pipe_out[0] = fd2[0];
		command.fd_pipe_out[1] = fd2[1];
		command.envp = envp;
		command.fd_file_in = fd_file_in;
		command.fd_file_out = fd_file_out;

		return (exec_proc(command));
	}

	close(fd[0]);
	close(fd[1]);

	pid[2] = fork();

	if (pid[2] == 0)
	{
		// Terceiro comando
		t_command	command;

		command.command = argv[3];
		command.type = LAST;
		command.fd_pipe_in[0] = fd2[0];
		command.fd_pipe_in[1] = fd2[1];
		command.envp = envp;
		command.fd_file_in = fd_file_in;
		command.fd_file_out = fd_file_out;

		return (exec_proc(command));
	}

	close(fd2[0]);
	close(fd2[1]);

	close(fd_file_in);
	close(fd_file_out);

	waitpid(pid[0], NULL, 0);
	waitpid(pid[1], NULL, 0);
	waitpid(pid[2], &error, 0);

	return ((error >> 8) & 0xFF);
}
