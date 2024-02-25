/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join_paths.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 12:59:22 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/24 13:15:21 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*join_paths(char *absolute, char *relative)
{
	char	total_size;
	int		absolute_size;
	char	*str;

	absolute_size = ft_strlen(absolute);
	if (absolute[absolute_size - 1] == '/')		// Se o nome do caminho absoluto ter uma barra '/' no final
		absolute_size--;							// Tira a barra '/' da contagem
	if (relative[0] == '/')						// Se o camiinho relativo tiver uma barra '/' no começo
		relative++;									// Incrimenta 1 na string para desconsiderar a barra '/'
	total_size = absolute_size + ft_strlen(relative) + 2;
	str = malloc(total_size);
	ft_strlcpy(str, absolute, total_size);
	str[absolute_size] = '/';
	str[absolute_size + 1] = '\0';
	ft_strlcat(str, relative, total_size);
	return (str);
}

int main()
{
	char *absolute = "./";
	char *relative = "/";
	char *expected_result = "./";
	char *result = join_paths(absolute, relative);

	printf("result: %s\n", result);

	if (strcmp(result, expected_result) == 0) {
		printf("Test passed!\n");
	} else {
		printf("Test failed!\n");
	}

	free(result);

	return 0;
}
