#ifndef FORMAT_H_
 #define FORMAT_H_

#include "structs.h"

void	draw_box(void);
void	clear_main_box(void);
void	clear_parent_box(void);
void	clear_sub_box(void);
void	clear_gutter(void);
void	clear_header(void);
void	spawn_popup(char *title, int lines);
void	clear_popup(int lines);

void	colour_entry(entry_node *entry);
void	format_entry(vd_node *dir_node, entry_node *current, entry_node *selected, int level);
void	print_entries(vd_node *dir_node, entry_node *selected, int level);
void	display_subdirectory(entry_node *selected, char *path);
void	display_parent(vd_node *dir_node, vd_node *parent);
void	preview_text(entry_node *file, int start_line);
void	display_directory(vd_node *dir_node, entry_node *selected, vd_node *parent, int preview_start);

#endif // !FORMAT_H_
