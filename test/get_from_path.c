/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_from_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 17:12:06 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/18 18:42:20 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// PATH
// PWD
// Home

char	**get_split(char *str)
{
	char	*start;
	char	**split;

	start = ft_strchr(str, '=') + 1;
	split = ft_split(start, ':');
	return (split);
}

char	*join_paths(char *absolute, char *relative)
{
	char	total_size;
	int		absolute_size;
	char	*str;

	absolute_size = ft_strlen(absolute);
	total_size = absolute_size + ft_strlen(relative) + 2;		// Mais 2 da "/" e do '\0'
	str = malloc(total_size);
	ft_strlcpy(str, absolute, total_size);
	str[absolute_size] = '/';
	str[absolute_size + 1] = '\0';
	ft_strlcat(str, relative, total_size);
	return (str);
}

int main(int argv, char *argc[], char *envp[])
{
	char	*pwd;
	char	*home;
	char	**path;
	char	*command;
	char	**command_splitted;

	if (argv != 2)
		return (1);

	command_splitted = ft_split(argc[1], ' ');

	for (int i = 0; envp[i]; i++)
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
			path = get_split(envp[i]);
		else if (ft_strncmp(envp[i], "PWD", 3) == 0)
			pwd = ft_strdup(ft_strchr(envp[i], '=') + 1);
		else if (ft_strncmp(envp[i], "HOME", 3) == 0)
			home = ft_strdup(ft_strchr(envp[i], '=') + 1);
	}

// 	printf("PATH: \n");
// 	for (int i = 0; path[i]; i++)
// 		printf("%s\n", path[i]);

	// printf("PWD = %s\n", pwd);
	// printf("HOME = %s\n", home);

	if (ft_strchr(command_splitted[0], '/') == NULL && command_splitted[0][0] != '~')
	{
		for (int i = 0; path[i]; i++)
		{
			command = join_paths(path[i], command_splitted[0]);
			printf("%s\n", command);
			if (access(command, F_OK | X_OK) == 0)
				break ;
			free(command);
			command = NULL;
		}
		// if (command == NULL)
		// 	command = ft_strdup(command_splitted[0]);
	}
	else
	{
		// int command_size = ft_strlen(command_splitted[0]);
		// if (command_splitted[0][command_size - 1] == '/')
		// 	command_splitted[0][command_size - 1] = '\0';

		if (command_splitted[0][0] == '/')
			command = ft_strdup(command_splitted[0]);
		else if (command_splitted[0][0] == '~')
		{
			if (command_splitted[0][1] == '/')
				command = join_paths(home, command_splitted[0] + 2);
			else
				command = join_paths(home, command_splitted[0] + 1);
		}
		else if (ft_strncmp(command_splitted[0], "./", 2) == 0)
			command = join_paths(pwd, command_splitted[0] + 2);
		else
			command = join_paths(pwd, command_splitted[0]);
	}

	if (command)
		printf("%s\n", command);

	ft_free_split(path);
	free(pwd);

	if (command && access(command, F_OK | X_OK) == 0)
		execve(command, command_splitted, envp);

	if (command || access(command, F_OK) != 0)
		perror("Command not found");
	else if (access(command, X_OK) != 0)		// Pareque que não pode usar isso: || errno == EACCES)
		perror("Permission denied");

	if (command)
		free(command);
	ft_free_split(command_splitted);

	return (0);
}

// testes
//
// ./a.out "pasta/"
// ./a.out "~"
// ./a.out "~/"
// ./a.out "cat/"
// ./a.out "."
