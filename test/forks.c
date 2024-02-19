/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 22:38:49 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/18 23:39:47 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

int main()
{
	int pid;

	puts("Comaçando!");

	pid = fork();

	if (pid != 0)
	{
		printf("%5d - Bom dia\n", pid);
		pid = fork();
	}
	else
		printf("%5d - Batata\n", pid);

	printf("%5d - Alo\n", pid);


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
