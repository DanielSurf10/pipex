/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_error_handling.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 20:27:33 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/12 21:20:58 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[], char *envp[])
{
	int		pid;

	if (argc != 2)
	{
		write(2, "Usage error.\nExpected: ./pipex <cmd>\n", 38);
		return (1);
	}

	char	*args[] = {argv[1], NULL};

	pid = fork();
	if (pid == 0)
	{
		int	execve_return = execve(args[0], args, envp);
		printf("errno = %d\nexecve = %d\n", errno, execve_return);
		perror(args[0]);

		if (errno == ENOENT || errno == ENOEXEC)
			return (127);
		else if (errno == EACCES)
			return (126);

		return (1);
	}

	int error_code;
	int	wait_return;

	wait_return = wait(&error_code);

	printf("\nerror: %d\nerro arrumado: %d\nwait: %d\n", error_code, (error_code >> 8) & 0xFF, wait_return);

	return ((error_code >> 8) & 0xFF);
}

// ./test /bin/ls
// ./test /bin/grep
// ./test not_found
// ./test ./files_test/file.txt			# colocar o caminho absoluto
// ./test ./files_test/file_exec.txt	# com permissão de execução
// ./test ./without_exec_permission		# aqui é um programa
// ./test ./files_test/no_permission
// ./test ./files_test					# testar com o caminho de uma pasta
