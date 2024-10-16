#include <stdlib.h>
#include <string.h>

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

char	*get_extension(char *ext_buf, char *file_name)
{
	int		i;

	i = strlen(file_name) - 1;
	while (i > 0 && file_name[i] != '.')
		i--;
	if (i == 0)
		return NULL;
	strcpy(ext_buf, &file_name[i + 1]);
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
