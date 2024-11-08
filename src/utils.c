#include "headers/utils.h"
#include <stdio.h>
#include <sys/stat.h>

int	str_printable(char *s)
// Checks if a string contains only printable characters.
// Args:
//  - s:	string to check
// Returns:
//  - 1 if string contains only printable characters
//  - 0 if string contains non-printable characters
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
// Checks if file at a given path is executable.
// Args:
//  - path:	relative path of file
// Returns:
//  - 1 if file is executable
//  - 0 if file is not executable
{
	struct stat	fs;

	if (stat(path, &fs) == 1)
		return (0);
	if (fs.st_mode & S_IXUSR)
		return (1);
	return (0);
}

int is_binary(char *path)
// Checks if a file is not a text file.
// Args:
//  - path:	relative path of file
// Returns:
//  - 1 if file is not a text file
//  - 0 if file is a text file
{
	FILE	*fileptr;
	char	buffer[500];
	int		i;
	int		n_read;

	fileptr = fopen(path, "rb");
	if (fileptr == NULL)
		return (1);
	while ((n_read = fread(buffer, 1, 500, fileptr)) != 0)
	{
		i = 0;
		while (i < n_read)
		{
			if (( buffer[i] < 32 || buffer[i] > 126) && buffer[i] != '\n' && buffer[i] != '\t')
			{
				fclose(fileptr);
				return (1);
			}
			i++;
		}
	}
	fclose(fileptr);
	return (0);
}

char	*get_extension(char *file_name)
// Retrieves the file extension of a file
// Args:
//  - file_name:	name of the file
// Returns:
//  - Memory allocated string containing file extension
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
// For some reason I did my own strlen implementation.
// Args:
//  - s:	string from which to get the length
// Returns:
//  - length of string
{
	int	i;

	i = 0;
	while (*s++ != '\0')
		i++;
	return (i);
}

char	*replace_tab(char *string, size_t size)
// Replaces tabs in a memory allocated string with 4 spaces.
// Args:
//  - string:	memory allocated string whose tabs will be replaced
//  - size:		maximum size of output string
// Returns:
//  - Memory allocated string with tabs replaced
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

int count_lines(char *path)
// Counts the number of lines in a text file.
// Args:
//  - path:	relative path of file
// Returns:
//  - Number of lines in file
//  - (-1) if unable to open file
{
	FILE	*fileptr;
	char	buffer[500];
	int		i;
	int		n_read;
	int		lines = 0;

	fileptr = fopen(path, "rb");
	if (fileptr == NULL)
		return (-1);
	while ((n_read = fread(buffer, 1, 500, fileptr)) != 0)
	{
		i = 0;
		while (i < n_read)
		{
			if (buffer[i] == '\n')
				lines++;
			i++;
		}
	}
	fclose(fileptr);
	return (lines);
}

int	count_digits(int num)
//	Counts the number of characters in an integer.
//	Args:
//	 - num:	the integer to count characters of
//	Returns:
//	 - The number of characters required to print the integer
{
	int	digits = 0;

	if (num == 0)
		return (1);
	if (num < 0)
		digits++;
	while (num != 0)
	{
		digits++;
		num /= 10;
	}
	return (digits);
}
