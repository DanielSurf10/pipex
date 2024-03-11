/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 20:44:08 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/11 15:55:09 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>

# include "libft.h"

// enums

enum e_fd
{
	READ_FD = 0,
	WRITE_FD
};

enum e_process
{
	FIRST = 0,
	MID,
	LAST
};

enum e_status
{
	FAIL = -1,
	READ,
	BUILD_STRING,
	FINISH
};

// structs

typedef struct s_path
{
	char	*home;
	char	**path;
}	t_path;

typedef struct s_pipe
{
	int	fd_pipe[2];
}	t_pipe;

typedef struct s_command
{
	int		i;
	int		fd_file_in;
	int		fd_file_out;
	int		num_cmds;
	int		*pid;
	t_pipe	*pipes;
	char	**argv;
	char	**envp;
	t_path	path;
}	t_command;

typedef struct s_list
{
	char			content;
	struct s_list	*next;
}	t_list;

// Functions

// Process
void	exec_process(t_command command, int type, int cmd_num, int relative);
void	close_pipes(t_command command, int cmd_num);

// Path
t_path	get_path_variables(char **envp);
char	*expand_path(char *cmd, t_path path);

// Here_doc
int		get_from_here_doc(char *delimiter);

// Utils
void	close_pipe(t_pipe pipe_to_close);
void	set_dup2(int fd_in, int fd_out);
void	free_all(t_command command);
char	*get_next_line(int fd);
int		ft_lstadd_back(t_list **lst, char c);
void	ft_lstclear(t_list **lst);
void	ft_lstclear(t_list **lst);
int		ft_lst_next_line_size(t_list *lst);

#endif
