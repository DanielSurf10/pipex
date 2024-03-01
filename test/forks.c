/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 22:38:49 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/01 13:58:10 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main()
{
	int pid;
	int ret_code;

	puts("Processo pai!");

	pid = fork();

	if (pid == 0)
	{
		puts("processo filho!");
		return (12);
	}

	waitpid(pid, &ret_code, 0);

	printf("pid: %5d\nreturn: %d\n", pid, (ret_code >> 8) & 0xff);

	return (0);
}

// set follow-fork-mode child		// Para seguir o child
// set follow-fork-mode parent		// Para seguir o parent
// show follow-fork-mode			// Para ver o valor

// set detach-on-fork on			// Para deixar rolando sozinho
// set detach-on-fork off			// Para depurar todo mundo junto
// show detach-on-fork				// Para ver o valor

// info inferiors					// para mostrar os processos ativos, que dá pra depurar
									// Aqui deve aparecer uma lista dos processos
									// Ou um processo só se não configurou "set detach-on-fork off"
									// Ou não passou por nenhum fork
// inferior <num>					// Para ecolher qual processo depurar
