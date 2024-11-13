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
	char			SORT;
	unsigned char	FLAGS;
} ;

enum	{F_PREVIEW = 1, F_HIDDEN = 2, F_REVERSE = 4};
enum	{SORT_ALPHA = 0, SORT_SIZE = 1, SORT_MTIME = 2, SORT_ATIME = 3};

struct bind_struct
{
	char	QUIT;
	char	UPDIR;
	char	EDIT_DIR;
	char	EDIT_FILE;
	char	TOGGLE_HIDDEN;
	char	TOGGLE_PARENT;
	char	PICK_SORT;
	char	EXEC_SHELL;
	char	HELP;
	char	PASTE;
	char	CLEAR_BUF;
	char	SELECT_NEXT;
	char	SELECT_PREV;
	char	SEARCH_NEXT;
	char	SEARCH_PREV;
	char	OPEN;
	char	YANK;
	char	CUT;
	char	EXEC_FILE;
	char	SEARCH_DIR;
	char	PREV_DOWN;
	char	PREV_UP;
	char	GO_FIRST;
	char	GO_LAST;
	char	DELETE;
	char	RENAME;
};

union bind_u
{
	struct bind_struct bstruct;
	char barr[sizeof(struct bind_struct)];
} ;

extern struct env_struct	env;
extern struct bind_struct	binds;

void	set_default_binds(void);
int		load_config(void);
int		check_double_binds(void);
void	set_winsize(void);
void	store_term_settings(void);
void	set_term_settings(void);
void	reset_term_settings(void);

#endif // !ENV_H_
