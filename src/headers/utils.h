#ifndef UTILS_H_
 #define UTILS_H_

#include <stdlib.h>
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define Apply_reverse(x) env.FLAGS & F_REVERSE ? !x : x

gen_node *init_gen_list(void);
void	delete_next_gen(gen_node *t);
char	*get_extension(char *file_name);
int		is_executable(char *path);
char	*replace_tab(char *string, size_t size);
int		my_strlen(char *s);
int		str_printable(char *s);
int		count_lines(char *path);
int 	is_binary(char *path);
int		count_digits(int num);
int		my_atoi(char *s);
int		is_digit(char c);
int		is_whitespace(char c);

#endif // !UTILS_H_
