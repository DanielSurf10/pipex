/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_bidirecional.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 15:50:03 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/01 14:00:12 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main()
{
	// Dois pipes, um para cada processo
	int	p1[2];	// C -> P
	int	p2[2];	// P -> C

	pipe(p1);
	pipe(p2);

	int	pid = fork();

	int	num;

	if (pid == 0)
	{
		// Processo filho
		close(p1[0]);		// Não precisa ler nese fd
		close(p2[1]);		// Não precisa escrever nesse fd

		read(p2[0], &num, sizeof(int));		// Lê no pipe do pai

		printf("Processo filho: %d\n", num);
		num *= 3;

		write(p1[1], &num, sizeof(int));	// Escreve no pipe do filho

		close(p1[1]);		// Fechar os fd que sobraram
		close(p2[0]);
	}
	else
	{
		// Processo pai
		close(p1[1]);		// Não precisa escrever nesse fd
		close(p2[0]);		// Não precisa ler nese fd

		num = 1503;

		printf("Processo pai antes: %d\n", num);

		write(p2[1], &num, sizeof(int));	// Escrece no pipe do pai

		read(p1[0], &num, sizeof(int));		// Lê do pipe do filho

		printf("Processo pai depois: %d\n", num);

		close(p1[0]);		// Fechar os fd que sobraram
		close(p2[1]);
	}


	return (0);
}
