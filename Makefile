BUILD_DIR := ./build
SRC_DIR := ./src

CC = gcc

CFLAGS = -Wall -Wextra -Werror -g3 -O3

# SRC = $(SRC_DIR)/main.c $(SRC_DIR)/entries.c $(SRC_DIR)/directories.c $(SRC_DIR)/format.c $(SRC_DIR)/utils.c $(SRC_DIR)/box.c
# OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/entries.o $(BUILD_DIR)/directories.o $(BUILD_DIR)/format.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/box.o

SRC := $(shell find $(SRC_DIR) -type f -name *.c)
OBJ := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRC:.c=.o))

#Defauilt Make

NAME = fse

all: directories $(NAME) install

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
#	rm -f $(OBJ) $(NAME)

install: 
	cp ./fse ~/.local/bin/fse

re: fclean all
.PHONY: all clean fclean re
