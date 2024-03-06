# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: danbarbo <danbarbo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/27 15:54:27 by danbarbo          #+#    #+#              #
#    Updated: 2024/03/06 00:47:35 by danbarbo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= pipex
NAME_B	:= pipex_bonus
CFLAGS	:= -Wextra -Wall -Werror -g3
LIBFT	:= ./lib/libft

HEADERS	:= -I ./include \
			-I ${LIBFT}/include

LIBS	:= ${LIBFT}/libft.a

SRCS	:= ${shell find src -iname "*.c"}
OBJS	:= ${SRCS:src/%.c=obj/%.o}

SRCS_B	:= ${shell find src_bonus -iname "*.c"}
OBJS_B	:= ${SRCS_B:src_bonus/%.c=obj/%.o}

all: ${NAME}
bonus: ${NAME_B}

${NAME}: libft ${OBJS}
	@${CC} ${OBJS} ${LIBS} ${HEADERS} -o ${NAME}

${NAME_B}: libft ${OBJS_B}
	@${CC} ${OBJS_B} ${LIBS} ${HEADERS} -o ${NAME_B}

obj/%.o: src/%.c
	@mkdir -p ${dir $@}
	@${CC} ${CFLAGS} -o $@ -c $< ${HEADERS}
	@printf "Compiling: ${notdir $<}\n"

obj/%.o: src_bonus/%.c
	@mkdir -p ${dir $@}
	@${CC} ${CFLAGS} -o $@ -c $< ${HEADERS}
	@printf "Compiling: ${notdir $<}\n"

libft:
	@make -C ${LIBFT} all

clean:
	@rm -rf obj
	@make -C ${LIBFT} clean

fclean: clean
	@rm -f ${NAME}
	@rm -f ${NAME_B}
	@make -C ${LIBFT} clean

re: fclean all

.PHONY: all clean fclean re libft test
