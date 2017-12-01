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
CC_FLAGS = -Wall -Werror -Wextra -I$(LFT_PATH)/include -O3

LFT_PATH = ./libft
SRC_PATH = ./src/
INC_PATH = ./include $(LFT_PATH)/include
OBJ_PATH = ./obj/

SRC = $(addprefix $(SRC_PATH),$(SRC_NAME))
OBJ = $(addprefix $(OBJ_PATH),$(OBJ_NAME))
INC = $(addprefix -I,$(INC_PATH))

OBJ_NAME = $(SRC_NAME:.c=.o)
SRC_NAME = select.c

all: $(NAME)

$(NAME): $(OBJ)
	+$(MAKE) -C $(LFT_PATH)
	$(CC) -o $(NAME) $(OBJ) -L$(LFT_PATH) -lft -lncurses


$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CC_FLAGS) $(INC) -o $@ -c $<

clean:
	make -C $(LFT_PATH) clean
	rm -rf $(OBJ_PATH)

fclean: clean
	make -C $(LFT_PATH) fclean
	rm -f $(NAME)

check:
	@./test.sh

re: fclean all

.PHONY: all, $(NAME), clean, fclean, re
