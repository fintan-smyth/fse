#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int	str_printable(char *s)
{
	while (*s != 0)
	{
		if ((*s < 32) || (*s > 126))
			return (0);
		s++;
	}
	return (1);
}

int	is_executable(char *path)
{
	struct stat	fs;

	if (stat(path, &fs) == 1)
		return (0);
	if (fs.st_mode & S_IXUSR)
		return (1);
	return (0);
}

char	*get_extension(char *file_name)
{
	int		i;
	char	*ext_buf;

	i = strlen(file_name) - 1;
	while (i > 0 && file_name[i] != '.')
		i--;
	if (i == 0)
		return NULL;
	ext_buf = strdup(&file_name[i + 1]);
	return (ext_buf);
}

int	my_strlen(char *s)
{
	int	i;

	i = 0;
	while (*s++ != '\0')
		i++;
	return (i);
}

char	*replace_tab(char *string, size_t size)
{
	int		i;
	int		j;
	char	*output = malloc(size);

	i = 0;
	j = 0;
	while (j < (int) size && string[i] != 0)
	{
		if (string[i] == '\t')
		{
			output[j++] = ' ';
			output[j++] = ' ';
			output[j++] = ' ';
			output[j++] = ' ';
		}
		else
			output[j++] = string[i];
		i++;
	}
	output[j] = 0;
	free(string);
	return (output);
}

int	count_lines(char *path)
{
	size_t	size = 511;
	char	buf[size];
	char	*line = buf;
	int		line_no = 0;
	FILE	*fp;

	fp = fopen(path, "r");
	if (fp == NULL)
	{
		return (-1);
	}
	while ((getline(&line, &size, fp)) != -1)
	{
		line_no++;
	}
	fclose(fp);
	return (line_no);
}
