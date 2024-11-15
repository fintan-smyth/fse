#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "headers/env.h"

struct env_struct	env;
struct bind_struct	binds;

void	set_default_binds(void)
// Sets the program binds to their default state.
{
	binds.QUIT = 'q';
	binds.UPDIR = 'h';
	binds.EDIT_DIR = 'E';
	binds.EDIT_FILE = 'e';
	binds.TOGGLE_HIDDEN = 'H';
	binds.TOGGLE_PARENT = 'P';
	binds.PICK_SORT = 'z';
	binds.EXEC_SHELL = ':';
	binds.HELP = '?';
	binds.PASTE = 'p';
	binds.CLEAR_BUF = 'c';
	binds.SELECT_NEXT = 'j';
	binds.SELECT_PREV = 'k';
	binds.SEARCH_NEXT = 'n';
	binds.SEARCH_PREV = 'N';
	binds.OPEN = 'l';
	binds.YANK = 'y';
	binds.CUT = 'd';
	binds.EXEC_FILE = 'x';
	binds.SEARCH_DIR = '/';
	binds.PREV_DOWN = ']';
	binds.PREV_UP = '[';
	binds.GO_FIRST = 'g';
	binds.GO_LAST = 'G';
	binds.DELETE = 'D';
	binds.RENAME = 'r';
}

int	is_whitespace(char c)
// Checks if a character is whitepace.
// Args:
//  - c:	the character to check
// Returns:
//  - 0 if character is not whitespace
//  - 1 if character is space or tab
//  - 2 if character is newline
{
	if (c == ' ' || c == '\t')
		return (1);
	if (c == '\n')
		return (2);
	return (0);
}

char	*match_keyword(char *keyword)
// Retrieves the corresponding entry in the bind struct 
// for a bind keyword.
// Args:
//  - keyword:	a string containing the bind keyword
// Returns:
//  - pointer to char in the bind struct corresponding to that keyword
//  - NULL if no match is found
{
	if (strcmp("QUIT", keyword) == 0)
		return (&binds.QUIT);
	if (strcmp("UPDIR", keyword) == 0)
		return (&binds.UPDIR);
	if (strcmp("EDIT_DIR", keyword) == 0)
		return (&binds.EDIT_DIR);
	if (strcmp("EDIT_FILE", keyword) == 0)
		return (&binds.EDIT_FILE);
	if (strcmp("TOGGLE_HIDDEN", keyword) == 0)
		return (&binds.TOGGLE_HIDDEN);
	if (strcmp("TOGGLE_PARENT", keyword) == 0)
		return (&binds.TOGGLE_PARENT);
	if (strcmp("PICK_SORT", keyword) == 0)
		return (&binds.PICK_SORT);
	if (strcmp("EXEC_SHELL", keyword) == 0)
		return (&binds.EXEC_SHELL);
	if (strcmp("HELP", keyword) == 0)
		return (&binds.HELP);
	if (strcmp("PASTE", keyword) == 0)
		return (&binds.PASTE);
	if (strcmp("PASTE", keyword) == 0)
		return (&binds.PASTE);
	if (strcmp("YANK", keyword) == 0)
		return (&binds.YANK);
	if (strcmp("CUT", keyword) == 0)
		return (&binds.CUT);
	if (strcmp("CLEAR_BUF", keyword) == 0)
		return (&binds.CLEAR_BUF);
	if (strcmp("SELECT_NEXT", keyword) == 0)
		return (&binds.SELECT_NEXT);
	if (strcmp("SELECT_PREV", keyword) == 0)
		return (&binds.SELECT_PREV);
	if (strcmp("SEARCH_NEXT", keyword) == 0)
		return (&binds.SEARCH_NEXT);
	if (strcmp("SEARCH_PREV", keyword) == 0)
		return (&binds.SEARCH_PREV);
	if (strcmp("OPEN", keyword) == 0)
		return (&binds.OPEN);
	if (strcmp("EXEC_FILE", keyword) == 0)
		return (&binds.EXEC_FILE);
	if (strcmp("SEARCH_DIR", keyword) == 0)
		return (&binds.SEARCH_DIR);
	if (strcmp("PREV_DOWN", keyword) == 0)
		return (&binds.PREV_DOWN);
	if (strcmp("PREV_UP", keyword) == 0)
		return (&binds.PREV_UP);
	if (strcmp("GO_FIRST", keyword) == 0)
		return (&binds.GO_FIRST);
	if (strcmp("GO_LAST", keyword) == 0)
		return (&binds.GO_LAST);
	if (strcmp("DELETE", keyword) == 0)
		return (&binds.DELETE);
	if (strcmp("RENAME", keyword) == 0)
		return (&binds.RENAME);
	return (NULL);
}

int	check_double_binds(void)
// Checks the bind struct to see if any keys are assigned to multiple commands.
// Returns:
//  - 0 if all commands have unique keys
//  - 1 if a key is assigned to multiple commands
{
	union bind_u	binds_u;
	int				no_binds = (int) sizeof(binds);
	int				i;
	int				j;

	binds_u.bstruct = binds;
	i = 0;
	printf("\e[2J\e[H");
	while (i < no_binds)
	{
		j = i + 1;
		while (j < no_binds)
		{
			if (binds_u.barr[i] == binds_u.barr[j])
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	load_config(void)
// Reads the config file located at ~/.config/fse/ and applies the
// bind options contained within.
// Returns:
//  - 0 if config loaded without error or does not exist
//  - 1 if config contains errors
{
	struct stat	fs;
	FILE		*fileptr;
	size_t		size = 500;
	char		*line;
	char		path[120];
	char		keyword[20];
	char		*home = getenv("HOME");
	char		*keybind;
	char		key;
	int			i;
	int			j;
	
	strncpy(path, home, 100);
	strcat(path, "/.config/fse/config");
	if (stat(path, &fs) == 1)
	{
		return (0);
	}
	fileptr = fopen(path, "r");
	if (fileptr == NULL)
	{
		return (0);
	}
	line = malloc(size);
	while (getline(&line, &size, fileptr) != -1)
	{
		printf("%s", line);
		i = 0;
		while (is_whitespace(line[i]))
			i++;
		if (line[i] == '#' || line[i] == 0)
			continue;
		j = 0;
		while (!is_whitespace(line[i]) && line[i] != '=')
		{
			if (line[i] == 0)
			{
				free(line);
				fclose(fileptr);
				return (1);
			}
			keyword[j++] = line[i++];
		}
		keyword[j] = 0;
		while (is_whitespace(line[i]) == 1)
			i++;
		if (line[i++] != '=')
		{
			free(line);
			fclose(fileptr);
			return (1);
		}
		while (is_whitespace(line[i]) == 1)
			i++;
		key = line[i++];
		if (key < 32 || key > 126)
		{
			free(line);
			fclose(fileptr);
			return (1);
		}
		while (is_whitespace(line[i]))
			i++;
		if (line[i] != 0)
		{
			free(line);
			fclose(fileptr);
			return (1);
		}
		keybind = match_keyword(keyword);
		if (keybind == NULL)
		{
			free(line);
			fclose(fileptr);
			return (1);
		}
		*keybind = key;
	}
	free(line);
	fclose(fileptr);
	return (0);
}

void	set_winsize(void)
// Sets global variables containing terminal dimensions
{
	struct winsize w;

	ioctl(0, TIOCGWINSZ, &w);
	env.TERM_COLS = w.ws_col;
	env.TERM_ROWS = w.ws_row;
	if (env.FLAGS & F_PREVIEW)
	{
		env.SEP_1 = env.TERM_COLS / 6;
		env.SEP_2 = env.TERM_COLS / 2;
	}
	else
	{
		env.SEP_1 = 1;
		env.SEP_2 = env.TERM_COLS / 3;
	}
}

void	store_term_settings(void)
// Stores the original terminal emulator attributes
{
	tcgetattr(STDIN_FILENO, &env.g_term_original);
}

void	set_term_settings(void)
// Sets the terminal attributes to those
// needed by the program:
//  - Disabling STDIN echo
//  - Processing single characters from STDIN as they appear
//  - Turning off cursor
{
	struct termios term;

	set_winsize();
	term = env.g_term_original;
	term.c_lflag &= ~ECHO;
	term.c_lflag &= ~(ICANON);
	tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25l");
}

void	reset_term_settings(void)
// Restores terminal attributes to their original state
{
	struct termios term;
	term = env.g_term_original;
	tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25h");
}
