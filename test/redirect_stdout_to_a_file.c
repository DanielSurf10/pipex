/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_stdout_to_a_file.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 16:07:01 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/11 16:11:20 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int	fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);

	dup2(fd, STDOUT_FILENO);
	close(fd);

	printf("Oi, faland pelo arquivo agora.\n");


	return (0);
}
