#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "headers/format.h"
#include "headers/env.h"

void	draw_box(void)
// Prints the frame border
{
	int	x;
	int	y;

	printf("\e[2J\e[H");
	// printf("\e[30m");
	y = 1;
	while (y <= env.TERM_ROWS - 1)
	{
		x = 1;
		while (x <= env.TERM_COLS)
		{
			printf("\e[%d;%dH", y, x);
			if (y == 2 || y == env.TERM_ROWS - 1)
				printf("%s", "─");
			else if (y != 1 && (x == 1 || x == env.TERM_COLS || x == env.SEP_1))
				printf("%s", "│");
			else if (x == (env.SEP_2) && y != 1)
				printf("%s", "│");
			else
				printf("%c", ' ');
			x++;
		}
		y++;
	}
	// printf("\e[;1H╭");
	printf("\e[2;%dH┤", env.TERM_COLS);
	printf("\e[%d;1H├", env.TERM_ROWS - 1);
	printf("\e[%d;%dH┤", env.TERM_ROWS - 1, env.TERM_COLS);
	printf("\e[2;%dH┬", (env.SEP_1));
	printf("\e[2;%dH┬", (env.SEP_2));
	printf("\e[2;1H├");
	if (env.FLAGS & F_PREVIEW)
		printf("\e[%d;%dH┴", env.TERM_ROWS - 1, (env.SEP_1));
	printf("\e[%d;%dH┴", env.TERM_ROWS - 1, env.SEP_2);
	printf("\e[m");
}

void	clear_main_box(void)
// Clears the box displaying the current directory
{
	int	row;
	int	width;
	int	start;

	width = (env.SEP_2 - env.SEP_1) - 1;
	start = env.SEP_1 + 1;
	row = 3;
	while (row < env.TERM_ROWS - 1)
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

	width = env.SEP_1 - 2;
	start = 2;
	row = 3;
	while (row < env.TERM_ROWS - 1)
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

	width = (env.TERM_COLS - env.SEP_2) - 1;
	start = env.SEP_2 + 1;
	row = 3;
	while (row < env.TERM_ROWS - 1)
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
	printf("\e[%d;1H╰", env.TERM_ROWS);
	i = 2;
	while (i++ < env.TERM_COLS)
		printf("─");
	printf("╯");
	printf("\e[m");
	fflush(stdout);
}

void	clear_header(void)
// Clears the "header" i.e the top row where the current 
// directory and selected file are displayed
{
	int	i;

	// printf("\e[30m");
	printf("\e[1;1H╭");
	i = 2;
	while (i++ < env.TERM_COLS)
		printf("─");
	printf("╮");
	printf("\e[m");
}

void	spawn_popup(char *title, int	lines)
// Spawns a popup window with a given number of lines.
// Args:
//  - lines:	the number of lines within the popup that can contain text
{
	int	i;

	printf("\e[%d;1H├", env.TERM_ROWS - (2 + lines));
	i = 2;
	while (i < env.TERM_COLS)
	{
		if ( i == env.SEP_1 || i == env.SEP_2)
			printf("┴");
		else
			printf("─");
		i++;
	}
	printf("┤");
	if (title != NULL)
		printf("\e[%d;3H[ \e[33;1m%s\e[m ]", env.TERM_ROWS - (2 + lines), title);
	if (env.FLAGS & F_PREVIEW)
		printf("\e[%d;%dH─", env.TERM_ROWS - 1, env.SEP_1);
	printf("\e[%d;%dH─", env.TERM_ROWS - 1, env.SEP_2);
	i = 0;
	while (i < lines)
	{
		printf("\e[%d;2H", env.TERM_ROWS - (2 + i));
		printf("%*s", env.TERM_COLS - 2, "");
		i++;
	}
}

void	clear_popup(int lines)
// Clears a popup window with a given number of lines.
// Args:
//  - lines:	the number of lines within the popup that can contain text
{
	int	i = 0;

	while (i < lines)
	{
		printf("\e[%d;2H", env.TERM_ROWS - (2 + i));
		printf("%*s", env.TERM_COLS - 2, "");
		i++;
	}
}
