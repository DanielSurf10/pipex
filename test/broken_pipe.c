/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   broken_pipe.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 17:53:51 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/23 10:20:59 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main() {
	int fd_pipe[2];
	char str[] = "Hello, world!";

	pipe(fd_pipe);

	close(fd_pipe[0]);				// Fecha o fd de leitura do pipe
	write(fd_pipe[1], str, 14);		// Quando tenta escrever no pipe dá SIGPIPE
										// Para escrever no pipe,
										// é necessário ter o fd de leitura aberto.
	return 0;
}
