/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_from_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 17:12:06 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/04 11:45:11 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

// PATH
// PWD
// Home

// typedef struct s_path
// {
// 	char	*home;
// 	char	*pwd;
// 	char	**path;
// }	t_path;

char	**split_path(char *str)
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
	if (absolute[absolute_size - 1] == '/')
		absolute_size--;
	if (relative[0] == '/')
		relative++;
	total_size = absolute_size + ft_strlen(relative) + 2;
	str = malloc(total_size);
	ft_strlcpy(str, absolute, total_size);
	str[absolute_size] = '/';
	str[absolute_size + 1] = '\0';
	ft_strlcat(str, relative, total_size);
	return (str);
}

t_path	get_path_variables(char **envp)
{
	int		i;
	t_path	path;

	i = 0;
	ft_bzero(&path, sizeof(t_path));
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
			path.path = split_path(envp[i]);
		else if (ft_strncmp(envp[i], "HOME", 3) == 0)
			path.home = ft_strdup(ft_strchr(envp[i], '=') + 1);
		i++;
	}
	return (path);
}

char	*expand_from_path(char *cmd, t_path path)
{
	int		i;
	char	*new_command;

	i = 0;
	new_command = NULL;
	while (path.path && path.path[i])
	{
		new_command = join_paths(path.path[i], cmd);
		if (access(new_command, F_OK | X_OK) == 0)
			break ;
		free(new_command);
		new_command = NULL;
		i++;
	}
	return (new_command);
}

char	*expand_path(char *cmd, t_path path)
{
	char	*new_command;

	new_command = NULL;
	// if (cmd[0] != '~' && ft_strncmp(cmd, ".", -1) != 0 && ft_strchr(cmd, '/') == NULL)
	if (ft_strchr(cmd, '/') == NULL)
		new_command = expand_from_path(cmd, path);
	else
	{
		if (cmd[0] == '~' && path.home != NULL)
			new_command = join_paths(path.home, cmd + 1);
		else
			new_command = ft_strdup(cmd);
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

	command = expand_path(command_splitted[0], path);

	if (command)
		printf("%s\n", command);

	ft_free_split(path.path);
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
