# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/27 15:54:27 by danbarbo          #+#    #+#              #
#    Updated: 2024/02/11 20:42:55 by danbarbo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= pipex
# CFLAGS	:= -Wextra -Wall -Werror -g3
CFLAGS	:= -g3
LIBFT	:= ./lib/libft

HEADERS	:= -I ./include \
			-I ${LIBFT}/include

LIBS	:= ${LIBFT}/libft.a

SRCS	:= $(shell find src -iname "*.c")
OBJS	:= ${SRCS:src/%.c=obj/%.o}

all: $(NAME)
$(NAME): libft $(OBJS)
	@$(CC) $(OBJS) $(LIBS) $(HEADERS) -o $(NAME)

obj/%.o: src/%.c
	@mkdir -p ${dir $@}
	@$(CC) $(CFLAGS) -o $@ -c $< $(HEADERS)
	@printf "Compiling: $(notdir $<)\n"

libft:
	@make -C ${LIBFT} all

clean:
	@rm -rf $(OBJS)
	@make -C ${LIBFT} clean

fclean: clean
	@rm -rf $(NAME)
	@make -C ${LIBFT} clean

re: fclean all

.PHONY: all clean fclean re libft
