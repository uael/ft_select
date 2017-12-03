# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/11/07 09:52:36 by alucas-           #+#    #+#              #
#    Updated: 2017/11/18 19:59:34 by null             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_select
CC = gcc
CFLAGS = -Werror -Wextra -Wall -O3

SRC_PATH = ./src/
OBJ_PATH = ./obj/
LIB_PATH = ./libft/
INC_PATH = ./include/ $(addprefix $(LIB_PATH), include/)
LNK_PATH = ./ $(LIB_PATH)

SRC_NAME = select.c
OBJ_NAME = $(SRC_NAME:.c=.o)
LIB_NAME = ft ncurses

SRC = $(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))
INC = $(addprefix -I, $(INC_PATH))
LNK = $(addprefix -L, $(LNK_PATH))
LIB = $(addprefix -l, $(LIB_NAME))

all: $(NAME)

$(NAME): $(OBJ)
	@make -C $(LIB_PATH)
	@$(CC) $(CFLAGS) $(LNK) $(LIB) $(INC) $(OBJ) -o $(NAME)
	@echo  "$(NAME): \033[32m[✔]\033[0m"

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	@echo "$(NAME): \c"
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
	@echo "\033[A\033[K\033[A"

clean:
	@rm -f $(OBJ)
	@rmdir $(OBJ_PATH) 2> /dev/null || echo "" > /dev/null

fclean: clean
	@rm -f $(NAME)

check:
	@./test.sh

re: fclean all
