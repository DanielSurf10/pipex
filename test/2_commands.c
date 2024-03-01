/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_commands.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/29 16:59:19 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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

int main(int argc, char *argv[], char *envp[])
{
	int	fd_file_in;
	int	fd_file_out;
	int	fd[2];
	int	pid[2];
	int	error;

	if (argc != 5)
	{
		write(2, "Usage error.\nExpected: ./pipex <file_in> <cmd> <cmd> <file_out>\n", 64);
		return (1);
	}

	fd_file_in = open(argv[1], O_RDONLY);
	if (fd_file_in < 0)
		perror("Invalid input file");

	fd_file_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_file_out < 0)
		perror("Invalid output file");

	// Aqui dá leak de fd do fd_file_out, se der ruim, quando o arquivo de entrada não existir
	// if (access(argv[1], F_OK) != 0 || fd_file_out < 0)
	// 	return(1);

	// if (fd_file_in < 0)		// Se o arquivo existir e não tiver pemissão
	// 	return(0);

	pipe(fd);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		// Processo filho primeiro comando
		char **args = ft_split(argv[2], ' ');

		if (fd_file_in != -1)
		{
			dup2(fd_file_in, STDIN_FILENO);
			dup2(fd[1], STDOUT_FILENO);

			close(fd_file_in);
			close(fd_file_out);

			close(fd[0]);
			close(fd[1]);

			if (access(args[0], F_OK | X_OK) == 0)
				execve(args[0], args, envp);

			ft_free_split(args);

			// write(2, "Deu ruim 1\n", 11);
			perror("Commmand 1");

			if (access(args[0], F_OK | X_OK) != 0)
				return (127);
			else if (errno == EACCES)
				return (126);
		}
		else
			return (1);
	}

	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		char **args = ft_split(argv[3], ' ');

		dup2(fd[0], STDIN_FILENO);
		dup2(fd_file_out, STDOUT_FILENO);

		close(fd[0]);
		close(fd[1]);

		close(fd_file_in);
		close(fd_file_out);

		if (fd_file_out != -1 && access(args[0], F_OK | X_OK) == 0)
			execve(args[0], args, envp);

		ft_free_split(args);

		// write(2, "Deu ruim 2\n", 11);
		if (fd_file_in != -1)
		{
			perror("Commmand 2");

			if (access(args[0], F_OK | X_OK) != 0)
				return (127);
			else if (errno == EACCES)
				return (126);
		}
		else
			return (1);
	}

	waitpid(pid[0], NULL, 0);

	close(fd[0]);
	close(fd[1]);
	close(fd_file_in);
	close(fd_file_out);

	waitpid(pid[1], &error, 0);

	return ((error >> 8) & 0xFF);
}

// ./pipex /proc/$$/fd/0 "/bin/cat -e" "/bin/grep a" /proc/$$/fd/1
