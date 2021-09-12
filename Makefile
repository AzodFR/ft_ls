NAME = ft_ls

INC = includes/

SRC = srcs/

FLAGS = -Wall -Werror -Wextra

CC = gcc $(FLAGS)

OBJ = ft_ls.c

OBJS = $(addprefix $(OBJ), $(SRC))

all: $(NAME)

%.c : %.o, $(INC)/ft_ls.h
	$(CC) $^ 
$(NAME): $(INC)/*