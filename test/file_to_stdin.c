/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_to_stdin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 16:13:03 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/11 17:11:21 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd = open("test/test_file.txt", O_RDONLY);
	dup2(fd, STDIN_FILENO);
	close(fd);

	char read_buffer[16];
	int  readed;

	while ((readed = read(0, read_buffer, 16)) != 0)
	{
		read_buffer[readed] = '\0';
		printf("%s", read_buffer);
	}

	return (0);
}
