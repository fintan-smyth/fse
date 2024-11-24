#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/format.h"
#include "headers/env.h"
#include <string.h>

int	colour_extension(char *filename)
{
	char	*ext_buf;

	if ((ext_buf = get_extension(filename)) != NULL)
	{
		if (strcasecmp(ext_buf, "mp4") == 0
			|| strcasecmp(ext_buf, "mkv") == 0
			|| strcasecmp(ext_buf, "png") == 0
			|| strcasecmp(ext_buf, "jpg") == 0
			|| strcasecmp(ext_buf, "jpeg") == 0
			|| strcasecmp(ext_buf, "xcf") == 0
			|| strcasecmp(ext_buf, "tif") == 0
			|| strcasecmp(ext_buf, "gif") == 0
		) {
			printf("\e[35m");
			free(ext_buf);
			return (1);
		}
		else if (strcasecmp(ext_buf, "mp3") == 0
			|| strcasecmp(ext_buf, "flac") == 0
			|| strcasecmp(ext_buf, "m4a") == 0
		) {
			printf("\e[36m");
			free(ext_buf);
			return (1);
		}
		else if (strcasecmp(ext_buf, "xz") == 0
			|| strcasecmp(ext_buf, "gz") == 0
			|| strcasecmp(ext_buf, "zstd") == 0
			|| strcasecmp(ext_buf, "tar") == 0
			|| strcasecmp(ext_buf, "zip") == 0
		) {
			printf("\e[31m");
			free(ext_buf);
			return (1);
		}
		else
			free(ext_buf);
	}
	return (0);
}

void	colour_entry(entry_node *entry)
// Prints the appropriate escape code to colour an entry's
// file name according to formatting rules.
// Args:
//  - entry:	pointer to entry node of file
{
	printf("\e[m");
	switch (entry->data->d_type) {
		case DT_DIR:
			printf("\e[1;34m");
			break;
		case DT_LNK:
			printf("\e[1;36m");
			break;
		case DT_CHR:
			printf("\e[1;33m");
			break;
		case DT_BLK:
			printf("\e[1;33m");
			break;
		case DT_REG:
			if (colour_extension(entry->data->d_name) == 1)
				break;
			else if (entry->attr == NULL)
				break;
			else if (entry->attr->st_mode & S_IXUSR)
				printf("\e[1;32m");
			break;
		default:
			break;
	}
}

void	format_entry(vd_node *dir_node, entry_node *current, entry_node *selected, int level)
// Prints the formatted file name of an entry according to formatting rules.
// Args:
//  - dir_node:	pointer to node of directory containing entry
//  - current:	pointer to entry to format
//  - selected:	pointer to the selected entry in directory
//  - level:	level of directory containing entry:
//  			 > 0 if it is the cwd
//  			 > 1 if it is a subdirectory of cwd
//  			 > -1 if it is the parent directory of cwd
{
	char	buf[500] = {0};
	char	*search_term_start;
	int		offset;
	int		box_width;
	int		remaining_space;

	construct_path(buf, dir_node->dir_name, current->data->d_name);
	switch (level) {
		case 0:
			offset = env.SEP_1 + 2;
			box_width = (env.SEP_2 - env.SEP_1) - 3;
			break;
		case 1:
			box_width = (env.TERM_COLS - env.SEP_2) - 3;
			offset = env.SEP_2 + 2;
			break;
		case -1:
			box_width = env.SEP_1 - 4;
			offset = 3;
			break;
		default :
			offset = env.SEP_1 + 2;
			box_width = (env.SEP_2 - env.SEP_1) - 3;
			break;
	}
	printf("\e[%dG", offset);
	colour_entry(current);
	if (current == selected)
	{
		if (level == 1)
			printf("\e[4m");
		else
			printf("\e[1;7m");
	}
	printf("%.*s", box_width - 1, current->data->d_name);
	remaining_space = box_width - my_strlen(current->data->d_name);
	if (remaining_space > 0)
		printf("%*s", box_width - my_strlen(current->data->d_name), " ");
	search_term_start = strcasestr(current->data->d_name, dir_node->search_term);
	if (my_strlen(current->data->d_name) > box_width)
		printf("~");
	if (check_path(copied, buf))
		printf("\e[%dG\e[m\e[33m*\e[m", offset - 1);
	if (check_path(cut, buf))
		printf("\e[%dG\e[m\e[31m*\e[m", offset - 1);
	if (search_term_start != NULL && *dir_node->search_term != 0)
		printf("\e[%sm\e[%dG%.*s", current == selected ? "7;41" : "1;31", offset + (int)(search_term_start - current->data->d_name), my_strlen(dir_node->search_term), search_term_start);
}

void	print_entries(vd_node *dir_node, entry_node *selected, int level)
// Prints a formatted list of the entries in a directory.
// Args:
//  - dir_node:	pointer to directory node
//  - selected:	pointer to node of selected entry
//  - level:	level of directory:
//  			 > 0 if it is the cwd
//  			 > 1 if it is a subdirectory of cwd
//  			 > -1 if it is the parent directory of cwd
{
	entry_node			*head;
	entry_node			*current;
	int					offset;
	int 				max = env.TERM_ROWS - 4;
	int					i = 0;

	head = dir_node->directory->children;
	if (head->next == head->next->next)
		return ;
	current = head->next;
	offset = dir_node->offset;
	if (selected->pos > max + offset)
	{
		switch (level) {
			case 0:
				clear_main_box();
				break;
			case 1:
				clear_sub_box();
				break;
			case -1:
				clear_parent_box();
				break;
			default :
				break;
		}
		offset = selected->pos - max;
	}
	else if (selected->pos <= offset)
	{
		switch (level) {
			case 0:
				clear_main_box();
				break;
			case 1:
				clear_sub_box();
				break;
			case -1:
				clear_parent_box();
				break;
			default :
				break;
		}
		offset = selected->pos - 1;
	}
	dir_node->offset = offset;
	// while (current->pos != (offset + 1))
	// 	current = current->next;
	i = offset;
	printf("\e[3;1H");
	// while ((current != current->next) && current->pos <= max + offset)
	// {
	// 	format_entry(dir_node, current, selected, level);
	// 	printf("\n\e[m");
	// 	current = current->next;
	// }
	current = dir_node->entry_array[i];
	while (i < dir_node->no_entries && dir_node->entry_array[i]->pos <= max + offset)
	{
		current = dir_node->entry_array[i];
		format_entry(dir_node, current, selected, level);
		printf("\n\e[m");
		i++;
	}
}

void	display_subdirectory(vd_node *dir_node, entry_node *dir_entry, char *cwd_path)
// Prints a formatted list of the entries in a subdirectory of the cwd.
// Args:
//  - dir_entry:	pointer to entry node of the subdirectory
//  - path:			absolute path of cwd
{
	vd_node		*subdir_node;
	entry_node	*selected_sub;

	if (strncmp(cwd_path, "/", strlen(cwd_path)))
		cwd_path = strcat(cwd_path, "/");
	cwd_path = strcat(cwd_path, dir_entry->data->d_name);
	subdir_node = get_vd_node(VISITED_DIRS, cwd_path);
	if (!check_child_inserted(dir_node, subdir_node))
		insert_child_vd(dir_node->children, subdir_node);
	get_directory(subdir_node);
	if (subdir_node->directory == NULL)
	{
		return ;
	}
	selected_sub = get_selected(subdir_node);
	print_entries(subdir_node, selected_sub, 1);
	// cleanup_directory(subdir_node);
}

void	display_parent(vd_node *dir_node, vd_node *parent)
// Prints a formatted list of the entries in the parent directory of the cwd.
// Args:
//  - dir_node:	pointer to directory node of cwd
//  - parent:	pointer to node of parent directory
{
	entry_node	*selected;

	if (strncmp(dir_node->dir_name, "/", strlen(dir_node->dir_name)) == 0)
	{
		clear_parent_box();
		return ;
	}
	get_directory(parent);
	free(parent->selected_name);
	parent->selected_name = get_file_name(dir_node->dir_name);
	selected = get_selected(parent);
	print_entries(parent, selected, -1);
	// cleanup_directory(parent);
}


void	preview_text(entry_node *file, int preview_offset)
// Prints the contents of a text file in the preview box.
// Args:
//  - file:				pointer to entry node of file to preview
//  - preview_offset	number of lines to skip from start of file
{
	size_t	size = 511;
	char	*line = malloc(size);
	int		line_no = 2;
	int		to_skip = preview_offset;
	FILE	*fp;

	fp = fopen(file->data->d_name, "r");
	if (fp == NULL)
	{
		free(line);
		printf("\e[3;%dH\e[7m%.*s\e[m\n", env.SEP_2 + 2, (env.TERM_COLS - env.SEP_2) - 2, "Cannot preview file");
		return;
	}
	printf("\e[3;1H");
	while (line_no < env.TERM_ROWS - 2 && (getline(&line, &size, fp)) != -1)
	{
		if (to_skip-- > 0)
			continue;
		if (strlen(line) > 0)
		{
			if (line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = '\0';
		}
		line = replace_tab(line, size);
		printf("\e[%dG%.*s\n", env.SEP_2 + 2, (env.TERM_COLS - env.SEP_2) - 2, line);
		line_no++;

	}
	if (preview_offset > 0)
	{
		printf("\e[3;%dH\e[1;7m%*s", env.SEP_2 + 1, (env.TERM_COLS - env.SEP_2) - 1, "");
		printf("\e[%dG^\e[%dG%c\e[m", (env.TERM_COLS + env.SEP_2) / 2, env.TERM_COLS - 3, binds.PREV_UP);
	}
	if ((file->lines - preview_offset) > env.TERM_ROWS - 4)
	{
		printf("\e[%d;%dH\e[1;7m%*s", env.TERM_ROWS - 2, env.SEP_2 + 1, (env.TERM_COLS - env.SEP_2) - 1, "");
		printf("\e[%dGv\e[%dG%c\e[m", (env.TERM_COLS + env.SEP_2) / 2, env.TERM_COLS - 3, binds.PREV_DOWN);
	}
	free(line);
	fclose(fp);
}

void	print_entry_no(vd_node *dir_node, entry_node *selected)
// Prints the position of the current entry in the directory
// and the total number of entries in the gutter.
// Args:
//  - dir_node:	pointer to the node of the directory
//  - selected:	pointer to the node of the selected entry
{
	int	len;
	int	pos;
	int	no_entries = dir_node->no_entries;
	int	entries_digits;

	if (selected == NULL)
		pos = 0;
	else
		pos = selected->pos;
	entries_digits = count_digits(no_entries);
	len = 6 + (2 * entries_digits);
	printf("\e[%d;%dH[ \e[1;33m%0*d/%d\e[m ]",
		env.TERM_ROWS, (env.TERM_COLS - len), entries_digits, pos, no_entries);
}

void	print_header(entry_node *selected, char *cwd_name)
// Prints header showing cwd and selected file.
// Args:
//  - selected:			pointer to node of selected entry
//  - cwd_name:			string containing the path of cwd
{
	clear_header();
	printf("\e[1;3H[ \e[31;1m%.*s", env.TERM_COLS - 6, cwd_name);
	if (my_strlen(cwd_name) != 1)
		printf("/");
	if (selected != NULL)
	{
		colour_entry(selected);
		printf("%.*s", (env.TERM_COLS - 8 - my_strlen(cwd_name)), selected->data->d_name);
	}
	printf("\e[m ]");
}

void	print_gutter(vd_node *dir_node, entry_node *selected)
// Prints gutter, including file attribute display or status message.
// Args:
//  - dir_node:			pointer to node of current directory
//  - selected:			pointer to node of selected entry
{
	if (!(env.FLAGS & F_GUTTER_PUSHBACK))
	{
		if (selected != NULL)
		{
			printf("\e[%d;3H[ ", env.TERM_ROWS);
			print_file_attributes(selected);
			printf(" ]");
		}
	}
	else
	{
		printf("\e[%d;3H[ %.*s ]", env.TERM_ROWS, env.TERM_COLS - 10, env.gutter_pushback);
		env.FLAGS ^= F_GUTTER_PUSHBACK;
	}
	print_entry_no(dir_node, selected);
}

void	preview_entry(vd_node *dir_node, entry_node *selected, char *cwd_name, int preview_offset)
// Previews the selected entry with the appropriate function.
// Args:
//  - selected:			pointer to node of selected entry
//  - cwd_name:			string containing the path of cwd
//  - preview_offset	number of lines to skip from start of text file preview
{
	clear_sub_box();
	if (selected->data->d_type == DT_DIR || selected->data->d_type == DT_LNK)
		display_subdirectory(dir_node, selected, cwd_name);
	else if (selected->data->d_type == DT_REG)
	{
		if (selected->lines == 0)
		{
			if (is_binary(selected->data->d_name))
			{
				selected->lines = -1;
				printf("\e[3;%dH\e[7m%.*s\e[m\n", env.SEP_2 + 2, (env.TERM_COLS - env.SEP_2) - 2, "Cannot preview file");
			}
			else
				selected->lines = count_lines(selected->data->d_name);
		}
		if (selected->lines < 0)
			printf("\e[3;%dH\e[7m%.*s\e[m\n", env.SEP_2 + 2, (env.TERM_COLS - env.SEP_2) - 2, "Cannot preview file");
		else
			preview_text(selected, preview_offset);
	}
}

void	display_directory(vd_node *dir_node, entry_node *selected, vd_node *parent, int preview_offset)
// Displays all information associated with current directory:
//  - Path of cwd and selected file
//  - Contents of cwd
//  - Attributes of selected entry
//  - Contents of selected subdirectory/text file
//  - Contents of parent directory (if enabled)
// Args:
//  - dir_node:			pointer to node of current directory
//  - selected:			pointer to node of selected entry
//  - parent:			pointer to node of parent directory
//  - preview_offset	number of lines to skip from start of text file preview
{
	int				size = 500 * sizeof(char);
	char			cwd_name[size];

	getcwd(cwd_name, size);
	print_entries(dir_node, selected, 0);
	if (env.FLAGS & F_PREVIEW)
		display_parent(dir_node, parent);
	print_header(selected, cwd_name);
	print_gutter(dir_node, selected);
	if (selected == NULL)
		return ;
	preview_entry(dir_node, selected, cwd_name, preview_offset);
}
