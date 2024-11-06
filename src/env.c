#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "headers/env.h"

struct env_struct	env;

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
