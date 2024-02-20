/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_from_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 17:12:06 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/20 11:24:52 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// PATH
// PWD
// Home

typedef struct s_path
{
	char	*home;
	char	*pwd;
	char	**path;
}	t_path;

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

t_path	get_path_variables(char *envp[])
{
	int		i;
	t_path	path;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
			path.path = get_split(envp[i]);
		else if (ft_strncmp(envp[i], "PWD", 3) == 0)
			path.pwd = ft_strdup(ft_strchr(envp[i], '=') + 1);
		else if (ft_strncmp(envp[i], "HOME", 3) == 0)
			path.home = ft_strdup(ft_strchr(envp[i], '=') + 1);
		i++;
	}
	return (path);
}

char	*get_command_from_path(char *cmd, t_path path)
{
	char	*new_command;

	new_command = NULL;
	if (ft_strchr(cmd, '/') == NULL && cmd[0] != '~')
	{
		for (int i = 0; path.path[i]; i++)
		{
			new_command = join_paths(path.path[i], cmd);
			printf("%s\n", new_command);
			if (access(new_command, F_OK | X_OK) == 0)
				break ;
			free(new_command);
			new_command = NULL;
		}
		// if (new_command == NULL)
		// 	new_command = ft_strdup(cmd);
	}
	else
	{
		// int command_size = ft_strlen(cmd);
		// if (cmd[command_size - 1] == '/')
		// 	cmd[command_size - 1] = '\0';

		if (cmd[0] == '/')
			new_command = ft_strdup(cmd);
		else if (cmd[0] == '~')
		{
			if (cmd[1] == '/')
				new_command = join_paths(path.home, cmd + 2);
			else
				new_command = join_paths(path.home, cmd + 1);
		}
		else if (ft_strncmp(cmd, "./", 2) == 0)
			new_command = join_paths(path.pwd, cmd + 2);
		else
			new_command = join_paths(path.pwd, cmd);
	}
	return (new_command);
}

int main(int argv, char *argc[], char *envp[])
{
	// char	*pwd;
	// char	*home;
	// char	**path;
	char	*command;
	char	**command_splitted;
	t_path	path;

	if (argv != 2)
		return (1);

	path = get_path_variables(envp);
	command_splitted = ft_split(argc[1], ' ');

// 	printf("PATH: \n");
// 	for (int i = 0; path[i]; i++)
// 		printf("%s\n", path[i]);

	// printf("PWD = %s\n", pwd);
	// printf("HOME = %s\n", home);

	command = get_command_from_path(command_splitted[0], path);

	if (command)
		printf("%s\n", command);

	ft_free_split(path.path);
	free(path.pwd);
	free(path.home);

	if (command && access(command, F_OK | X_OK) == 0)
		execve(command, command_splitted, envp);

	if (command || access(command, F_OK) != 0)
		perror("Command not found");
	else if (access(command, X_OK) != 0)		// Não sei se pode usar isso: || errno == EACCES)
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
