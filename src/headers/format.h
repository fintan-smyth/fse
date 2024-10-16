#ifndef FORMAT_H_
 #define FORMAT_H_

#include "structs.h"

extern int	TERM_COLS;
extern int	TERM_ROWS;
extern int	SEP_1;
extern int	SEP_2;

void	set_winsize(void);
void	set_term_settings(void);
void	reset_term_settings(void);
void	draw_box(void);
void	clear_main_box(void);
void	clear_parent_box(void);
void	clear_sub_box(void);

extern int	FLAG_HIDDEN;
extern int	FLAG_PREVIEW;

void	colour_entry(entry_node *entry);
void	format_entry(entry_node *current, entry_node *selected, int level);
void	print_entries(vd_node *dir_node, entry_node *selected, int level);
void	display_subdirectory(entry_node *selected, char *path);
void	display_parent(vd_node *dir_node, vd_node *parent);
void	preview_text(entry_node *file);
void	display_directory(vd_node *dir_node, entry_node *selected, vd_node *parent);

#endif // !FORMAT_H_
