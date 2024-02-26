/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 20:44:08 by danbarbo          #+#    #+#             */
/*   Updated: 2024/02/26 18:15:33 by danbarbo         ###   ########.fr       */
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
	LAST
};

// structs

typedef struct s_path
{
	char	*home;
	char	*pwd;
	char	**path;
}	t_path;

typedef struct s_pipex
{
	int		fd_file_in;
	int		fd_file_out;
	int		fd_pipe[2];
	int		pid[2];
	int		type;
	int		return_code;
	char	*command;
	char	**argv;
	char	**envp;
	t_path	path;
}	t_pipex;

// Functions

// main
int		exec_command(t_pipex command);

// path
t_path	get_path_variables(char **envp);
char	*get_absolute_path(char *cmd, t_path path);

// utils
void	close_pipe(int *fd_pipe);
void	set_dup2(int fd_in, int fd_out);

#endif
