#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
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
	binds.TOGGLE_HDN = 'H';
	binds.TOGGLE_PRVW = 'P';
	binds.TOGGLE_SORT = 'S';
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
	binds.PREV_DWN = ']';
	binds.PREV_UP = '[';
	binds.GO_FIRST = 'g';
	binds.GO_LAST = 'G';
	binds.DELETE = 'D';
	binds.RENAME = 'r';
}

void	set_winsize(void)
// Sets global variables containing terminal dimensions
{
	struct winsize w;

	ioctl(0, TIOCGWINSZ, &w);
	env.TERM_COLS = w.ws_col;
	env.TERM_ROWS = w.ws_row;
	if (env.FLAGS & F_PREVIEW)
		env.SEP_1 = env.TERM_COLS / 6;
	else
		env.SEP_1 = 1;
	env.SEP_2 = env.TERM_COLS / 2;
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
