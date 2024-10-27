#ifndef UTILS_H_
 #define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char	*get_extension(char *file_name);
int		is_executable(char *path);
char	*replace_tab(char *string, size_t size);
int		my_strlen(char *s);
int		str_printable(char *s);
int		count_lines(char *path);

#endif // !UTILS_H_
