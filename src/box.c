#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "headers/format.h"

int	TERM_COLS;
int TERM_ROWS;
int	SEP_1;
int	SEP_2;

void	set_winsize(void)
{
	struct winsize w;

    ioctl(0, TIOCGWINSZ, &w);
	TERM_COLS = w.ws_col;
	TERM_ROWS = w.ws_row;
	SEP_1 = TERM_COLS / 6;
	SEP_2 = TERM_COLS / 2;
}

void	set_term_settings(void)
{
	struct termios term;

	set_winsize();
    tcgetattr(fileno(stdin), &term);
    term.c_lflag &= ~ECHO;
	term.c_lflag &= ~(ICANON);
    tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25l");
}

void	reset_term_settings(void)
{
	struct termios term;
    tcgetattr(fileno(stdin), &term);
    term.c_lflag |= ECHO;
	term.c_lflag |= (ICANON);
    tcsetattr(fileno(stdin), TCSANOW, &term);
	printf("\e[?25h");
}
void	draw_box(void)
{
	int	x;
	int	y;

	printf("\e[2J\e[H");
	y = 1;
	while (y <= TERM_ROWS)
	{
		x = 1;
		while (x <= TERM_COLS)
		{
			printf("\e[%d;%dH", y, x);
			if (y == 2 || y == TERM_ROWS || ((y == 1) && (x < SEP_2)))
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
	printf("\e[;1H╭");
	printf("\e[2;%dH┐\e[1;%dH╮", TERM_COLS, (SEP_2));
	printf("\e[%d;1H└", TERM_ROWS);
	printf("\e[%d;%dH┘", TERM_ROWS, TERM_COLS);
	printf("\e[2;%dH┬", (SEP_1));
	printf("\e[2;%dH┼", (SEP_2));
	printf("\e[2;1H├");
	printf("\e[%d;%dH┴", TERM_ROWS, (SEP_1));
	printf("\e[%d;%dH┴", TERM_ROWS, SEP_2);
}

void	clear_main_box(void)
{
	int	x, y;

	y = 3;
	while (y < TERM_ROWS)
	{
		x = (SEP_1) + 1;
		while (x < (SEP_2))
		{
			printf("\e[%d;%dH ", y, x);
			x++;
		}
		y++;
	}
}

void	clear_parent_box(void)
{
	int	x, y;

	y = 3;
	while (y < TERM_ROWS)
	{
		x = 3;
		while (x < (SEP_1))
		{
			printf("\e[%d;%dH ", y, x);
			x++;
		}
		y++;
	}
}

void	clear_sub_box(void)
{
	int	row;
	int	width;
	int	start;

	width = (TERM_COLS - SEP_2) - 2;
	start = SEP_2 + 2;
	row = 3;
	while (row < TERM_ROWS)
	{
		printf("\e[%d;%dH%*s", row, start, width, "");
		row++;
	}
}
