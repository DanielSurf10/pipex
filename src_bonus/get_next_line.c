/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 14:24:21 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/11 15:26:09 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int	put_in_list(t_list **line, char *line_part, int size_readed)
{
	int	i;
	int	status_node_creation;
	int	return_status;

	i = 0;
	return_status = READ;
	if (!line_part)
		return (FAIL);
	if (size_readed == 0)
		return (BUILD_STRING);
	while (i < size_readed)
	{
		status_node_creation = ft_lstadd_back(line, line_part[i]);
		if (status_node_creation == FAIL)
			return (FAIL);
		if (line_part[i] == '\n')
			return_status = BUILD_STRING;
		i++;
	}
	return (return_status);
}

char	*build_line(t_list **line)
{
	int		i;
	int		line_size;
	char	*line_to_return;
	t_list	*aux;

	i = 0;
	if (*line == NULL)
		return (NULL);
	line_size = ft_lst_next_line_size(*line);
	line_to_return = (char *) malloc(line_size + 1);
	if (!line_to_return)
		return (NULL);
	line_to_return[line_size] = '\0';
	while (i < line_size)
	{
		aux = *line;
		line_to_return[i] = (*line)->content;
		*line = (*line)->next;
		free(aux);
		i++;
	}
	aux = NULL;
	return (line_to_return);
}

char	*read_fd(int fd, t_list **line)
{
	int		read_status;
	char	*line_part;
	char	*line_to_return;

	read_status = READ;
	line_part = NULL;
	line_to_return = NULL;
	if (!line)
		return (NULL);
	while (read_status == READ)
	{
		line_part = (char *) malloc(BUFFER_SIZE * sizeof(char));
		if (!line_part)
			return (NULL);
		read_status = read(fd, line_part, BUFFER_SIZE);
		if (read_status != FAIL)
			read_status = put_in_list(line, line_part, read_status);
		free(line_part);
		line_part = NULL;
	}
	if (read_status == FAIL)
		return (NULL);
	if (read_status == BUILD_STRING)
		line_to_return = build_line(line);
	return (line_to_return);
}

char	*get_next_line(int fd)
{
	char			*line_to_return;
	static t_list	*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	line_to_return = read_fd(fd, &line);
	if (!line_to_return)
		ft_lstclear(&line);
	return (line_to_return);
}
