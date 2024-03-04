/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 20:44:08 by danbarbo          #+#    #+#             */
/*   Updated: 2024/03/04 20:28:28 by danbarbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>		// Pode usar se for usar strerror
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>

# include "libft.h"

// enums

enum e_fd
{
	READ = 0,
	WRITE
};

enum e_process
{
	FIRST = 0,
	MID,
	LAST
};

// structs

typedef struct s_path
{
	char	*home;
	char	**path;
}	t_path;

typedef struct s_command
{
	int		i;
	int		fd_file_in;
	int		fd_file_out;
	int		num_cmds;
	int		*fd_pipes;
	int		*pid;
	char	**argv;
	char	**envp;
	t_path	path;
}	t_command;

// Functions

// main
void	exec_process(t_command command, int type, int cmd_num);

// path
t_path	get_path_variables(char **envp);
char	*expand_path(char *cmd, t_path path);

// utils
void	close_pipe(int *fd_pipe);
void	set_dup2(int fd_in, int fd_out);
void	free_all(t_command command);

#endif
