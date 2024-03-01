/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_and_access.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 14:01:40 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/01 14:11:10 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main()
{
	int fd_valid;
	int fd_invalid;
	int fd_file_not_exist;

	fd_valid = open("my_tester/infile", O_RDONLY);
	printf("Fd válido: %d\naccess: %d\n\n", fd_valid, access("my_tester/infile", F_OK));
	close(fd_valid);

	fd_file_not_exist = open("my_tester/not_exist", O_RDONLY);
	printf("Fd válido: %d\naccess: %d\n\n", fd_file_not_exist, access("my_tester/not_exist", F_OK));

	fd_invalid = open("files_test/no_permission.txt", O_RDONLY);
	printf("Fd válido: %d\naccess: %d\n\n", fd_invalid, access("files_test/no_permission.txt", F_OK));

	return (0);
}
