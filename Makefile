BUILD_DIR := ./build
SRC_DIR := ./src

CC = gcc

CFLAGS = -Wall -Wextra -Werror -g3 -O3

SRC := $(shell find $(SRC_DIR) -type f -name *.c)
OBJ := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRC:.c=.o))

NAME = fse

all: directories $(NAME)

directories: 
	mkdir -p build

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

$(OBJ): $(BUILD_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean: 
	rm -rf $(OBJ) $(BUILD_DIR)

fclean: clean	
	rm -f $(NAME)

install: 
	mkdir -p ~/.local/bin/
	mkdir -p ~/.local/share/fse/.trash
	mkdir -p ~/.config/fse/
	cp ./fse ~/.local/bin/fse
	cp ./config ~/.config/fse/config

re: fclean all
.PHONY: all clean fclean re
