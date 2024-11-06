#ifndef ENV_H_
 #define ENV_H_

#include <termios.h>

struct env_struct
{
	struct termios	g_term_original;
	int				TERM_COLS;
	int 			TERM_ROWS;
	int				SEP_1;
	int				SEP_2;
	unsigned char	FLAGS;
} ;

enum	{F_PREVIEW = 1, F_HIDDEN = 2, F_SORT = 4};

// extern int 					FLAG_SORT;
// extern int					FLAG_HIDDEN;
// extern int					FLAG_PREVIEW;
extern struct env_struct	env;

#endif // !ENV_H_
