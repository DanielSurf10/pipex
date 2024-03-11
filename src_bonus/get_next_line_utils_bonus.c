/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 15:26:24 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/11 15:26:39 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int	ft_lstadd_back(t_list **lst, char c)
{
	t_list	*new;
	t_list	*aux;

	new = malloc(sizeof(t_list));
	if (!new)
		return (FAIL);
	new->content = c;
	new->next = NULL;
	if (!(*lst))
		*lst = new;
	else
	{
		aux = *lst;
		while (aux->next)
			aux = aux->next;
		aux->next = new;
	}
	return (0);
}

void	ft_lstclear(t_list **lst)
{
	t_list	*aux1;
	t_list	*aux2;

	if (lst)
	{
		aux1 = *lst;
		while (aux1)
		{
			aux2 = aux1->next;
			free(aux1);
			aux1 = aux2;
		}
		*lst = NULL;
	}
}

int	ft_lst_next_line_size(t_list *lst)
{
	int		i;
	t_list	*aux;

	i = 0;
	aux = lst;
	while (aux)
	{
		i++;
		if (aux->content == '\n')
			return (i);
		aux = aux->next;
	}
	return (i);
}
