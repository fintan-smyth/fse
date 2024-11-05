#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "headers/format.h"

struct termios g_term_original;
int	TERM_COLS;
int TERM_ROWS;
int	SEP_1;
int	SEP_2;

void	set_winsize(void)
// Sets global variables containing terminal dimensions
{
	struct winsize w;

	ioctl(0, TIOCGWINSZ, &w);
	TERM_COLS = w.ws_col;
	TERM_ROWS = w.ws_row;
	if (FLAG_PREVIEW == 1)
		SEP_1 = TERM_COLS / 6;
	else
		SEP_1 = 1;
	SEP_2 = TERM_COLS / 2;
}

void	store_term_settings(void)
// Stores the original terminal emulator attributes
{
	tcgetattr(STDIN_FILENO, &g_term_original);
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
	term = g_term_original;
	term.c_lflag &= ~ECHO;
	term.c_lflag &= ~(ICANON);
	tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25l");
}

void	reset_term_settings(void)
// Restores terminal attributes to their original state
{
	struct termios term;
	term = g_term_original;
	tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25h");
}

void	draw_box(void)
// Prints the frame border
{
	int	x;
	int	y;

	printf("\e[2J\e[H");
	// printf("\e[30m");
	y = 1;
	while (y <= TERM_ROWS - 1)
	{
		x = 1;
		while (x <= TERM_COLS)
		{
			printf("\e[%d;%dH", y, x);
			if (y == 2 || y == TERM_ROWS - 1)
				printf("%s", "─");
			else if (y != 1 && (x == 1 || x == TERM_COLS || x == SEP_1))
				printf("%s", "│");
			else if (x == (SEP_2) && y != 1)
				printf("%s", "│");
			else
				printf("%c", ' ');
			x++;
		}
		y++;
	}
	// printf("\e[;1H╭");
	printf("\e[2;%dH┤", TERM_COLS);
	printf("\e[%d;1H├", TERM_ROWS - 1);
	printf("\e[%d;%dH┤", TERM_ROWS - 1, TERM_COLS);
	printf("\e[2;%dH┬", (SEP_1));
	printf("\e[2;%dH┬", (SEP_2));
	printf("\e[2;1H├");
	if (FLAG_PREVIEW == 1)
		printf("\e[%d;%dH┴", TERM_ROWS - 1, (SEP_1));
	printf("\e[%d;%dH┴", TERM_ROWS - 1, SEP_2);
	printf("\e[m");
}

void	clear_main_box(void)
// Clears the box displaying the current directory
{
	int	row;
	int	width;
	int	start;

	width = (SEP_2 - SEP_1) - 1;
	start = SEP_1 + 1;
	row = 3;
	while (row < TERM_ROWS - 1)
	{
		printf("\e[%d;%dH%*s", row, start, width, "");
		row++;
	}
}

void	clear_parent_box(void)
// Clears the box displaying the parent directory
{
	int	row;
	int	width;
	int	start;

	width = SEP_1 - 2;
	start = 2;
	row = 3;
	while (row < TERM_ROWS - 1)
	{
		printf("\e[%d;%dH%*s", row, start, width, "");
		row++;
	}
}

void	clear_sub_box(void)
// Clears the box previewing the contents of the selected file/directory
{
	int	row;
	int	width;
	int	start;

	width = (TERM_COLS - SEP_2) - 1;
	start = SEP_2 + 1;
	row = 3;
	while (row < TERM_ROWS - 1)
	{
		printf("\e[%d;%dH%*s", row, start, width, "");
		row++;
	}
}

void	clear_gutter(void)
// Clears the "gutter" i.e the bottom row where relevant 
// information/prompts are displayed
{
	int	i;
	
	// printf("\e[30m");
	printf("\e[%d;1H╰", TERM_ROWS);
	i = 2;
	while (i++ < TERM_COLS)
		printf("─");
	printf("╯");
	printf("\e[m");
}

void	clear_header(void)
// Clears the "header" i.e the top row where the current 
// directory and selected file are displayed
{
	int	i;

	// printf("\e[30m");
	printf("\e[1;1H╭");
	i = 2;
	while (i++ < TERM_COLS)
		printf("─");
	printf("╮");
	printf("\e[m");
}
