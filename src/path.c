/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:16:26 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/26 19:01:32 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
			path.path = split_path(envp[i]);
		else if (ft_strncmp(envp[i], "PWD", 3) == 0)
			path.pwd = ft_strdup(ft_strchr(envp[i], '=') + 1);
		else if (ft_strncmp(envp[i], "HOME", 3) == 0)
			path.home = ft_strdup(ft_strchr(envp[i], '=') + 1);
		i++;
	}
	return (path);
}

char	*get_from_path(char *cmd, t_path path)
{
	int		i;
	char	*new_command;

	i = 0;
	while (path.path[i])
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

char	*get_absolute_path(char *cmd, t_path path)
{
	char	*new_command;

	if (cmd[0] != '~' && ft_strncmp(cmd, ".", -1) != 0
		&& ft_strchr(cmd, '/') == NULL)
		new_command = get_from_path(cmd, path);
	else
	{
		if (cmd[0] == '/')
			new_command = ft_strdup(cmd);
		else if (ft_strncmp(cmd, "./", 2) == 0)
			new_command = join_paths(path.pwd, cmd + 2);
		else if (cmd[0] == '~')
			new_command = join_paths(path.home, cmd + 1);
		else
			new_command = join_paths(path.pwd, cmd);
	}
	return (new_command);
}
