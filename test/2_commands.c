/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_commands.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:12:50 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/20 10:12:21 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// Passos da execução
//
// 1 - Verifica se tem os 4 argumentos
//
// 2 - Verifica o arquivo de entrada - argv[1]
//		2.1 - Abre um fd para o arquivo de entrada como O_RDONLY
//		2.1 - Verifica se é um fd válido
//			2.1.1 - Se não for um erro é escrito no fd STDERR_FILENO
//
// 3 - Verifica o arquivo de saída - argv[argc - 1]
//		3.1 - Abre um fd para o arquivo de saída como O_WRONLY | O_CREAT | O_TRUNC, 0644
//			3.1.1 - O_WRONLY - O fd é apenas escrita
//			3.1.2 - O_CREAT  - Cria o arquivo se não existir
//			3.1.3 - O_TRUNC  - Apaga todo o conteúdo do arquivo se existir
//			3.1.4 - Muda a permissão do arquivo para 644
//				3.1.4.1 - S_IRUSR - Usuário tem permissão de leitura
//				3.1.4.2 - S_IWUSR - Usuário tem permissão de escrita
//				3.1.4.2 - S_IRGRP - Grupo tem permissão de leitura
//				3.1.4.2 - S_IROTH - Outros tem permissão de leitura
//			3.1.5 - Se o arquivo já existir ele muda a permissão do arquivo também
//		3.2 - Verifica se é um fd válido
//			3.2.1 - Se não for um erro é escrito no fd STDERR_FILENO
//
// 4 - Se os 2 fd's são válidos continua, se não retorna 1
//
// 5 - Cria o pipe
//
// 6 - Faz o primeiro fork
//		6.1 - No processo filho
//			6.1.1 - Fecha os fd's que não for usar
//				6.1.1.1 - fd[0] - pq não precisa ler nada do pipe
//				6.1.1.2 - fd_file_out - pq não precisa escrever nada no arquivo
//			6.1.2 - Faz um dup2 com fd_file_in e STDIN_FILENO
//				6.1.2.1 - Fecha o que era STDIN_FILENO
//				6.1.2.2 - Abre uma cópia do fd_file_in e atribui para STDIN_FILENO
//				6.1.2.3 - Isso faz a entrada do processo ser o arquivo de entrada
//			6.1.3 - Faz um dup2 com fd[1] e STDOUT_FILENO
//				6.1.2.1 - Fecha o que era STDOUT_FILENO
//				6.1.2.2 - Abre uma cópia do fd[1] e atribui para STDOUT_FILENO
//				6.1.2.3 - Isso faz a saída do processo ser o pipe
//			6.1.3 - Fecha os fd's fd[1] e fd_file_in, já que não precisa mais
//			6.1.4 - Verifica o primeiro comando (argv[2]) - access(argv[2], F_OK | X_OK) == 0
//				6.1.4.1 - Verifica se o caminho argv[2] é um arquivo ou pasta
//				6.1.4.2 - Verifica se o arquivo tem permissão de execução
//			6.1.5 - Executa o arquivo com execve
//				6.1.5.1 - Acaba por aqui o processo
//				6.1.5.1 - A menos que ocorra um problema na execução
//			6.1.6 - Se houver algum erro, o erro é escrito no fd STDERR_FILENO e retorna
//
// 7 - Cria o segundo pipe
//
// 8 - Faz o segundo fork

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

	if (fd_file_in < 0 || fd_file_out < 0)
		exit(1);

	pipe(fd);
	pid[0] = fork();

	if (pid[0] == 0)
	{
		// Processo filho primeiro comando
		char **args = ft_split(argv[2], ' ');

		close(fd[0]);
		close(fd_file_out);

		dup2(fd_file_in, STDIN_FILENO);
		close(fd_file_in);

		dup2(fd[1], STDOUT_FILENO);
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
		else
			return (1);
	}

	pid[1] = fork();

	if (pid[1] == 0)
	{
		// Processo filho segundo comando
		char **args = ft_split(argv[3], ' ');

		close(fd_file_in);
		close(fd[1]);

		dup2(fd_file_out, STDOUT_FILENO);
		close(fd_file_out);

		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);

		if (access(args[0], F_OK | X_OK) == 0)
			execve(args[0], args, envp);

		ft_free_split(args);

		// write(2, "Deu ruim 2\n", 11);
		perror("Commmand 2");

		if (access(args[0], F_OK | X_OK) != 0)
			return (127);
		else if (errno == EACCES)
			return (126);
		else
			return (1);
	}

	close(fd[0]);
	close(fd[1]);

	close(fd_file_in);
	close(fd_file_out);

	waitpid(pid[0], NULL, 0);
	waitpid(pid[1], &error, 0);

	return ((error >> 8) & 0xFF);
}

// int main(int argc, char *argv[])
// {
// 	int fd = -1;
// 	// if (access(argv[1], F_OK | R_OK) != 0)
// 	// 	perror("Error");
// 	if ((fd = open(argv[1], O_RDONLY)) < 0)
// 		perror("Error description");
// 	else if ((read(fd, 0, 0)) < 0)
// 		perror("Error desc");
// 	else
// 	{
// 		char lido[50];
// 		memset(lido, 0, 50);
//
// 		printf("%d\n", fd);
// 		printf("%s\n", lido);
// 	}
// }
