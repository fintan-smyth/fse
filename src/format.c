#include "headers/structs.h"
#include "headers/utils.h"
#include <string.h>
#include "headers/format.h"

int	FLAG_HIDDEN = 0;
int	FLAG_PREVIEW = 1;

void	colour_entry(entry_node *entry)
{
	char	ext_buf[10];

	switch (entry->data->d_type) {
		case DT_DIR:
			printf("\e[34m");
			break;
		case DT_LNK:
			printf("\e[32m");
			break;
		case DT_REG:
			if (get_extension(ext_buf, entry->data->d_name) == NULL)
			{
				printf("\e[39m");
				break;
			}
			else if (strcmp(ext_buf, "mp4") == 0
					|| strcmp(ext_buf, "mkv") == 0
					// || strcmp(ext_buf, "") == 0
				)
				printf("\e[36m");
			else
				printf("\e[39m");
			break;
		default:
			printf("\e[39m");
			break;
	}
}

void	format_entry(vd_node *dir_node, entry_node *current, entry_node *selected, int level)
{
	char	buf[500] = {0};
	char	*found;
	int		offset;
	int		box_width;

	switch (level) {
		case 0:
			offset = SEP_1 + 2;
			break;
		case 1:
			offset = SEP_2 + 2;
			break;
		case -1:
			offset = 2;
			break;
		default :
			break;
	}
	printf("\e[%dG", offset);
	colour_entry(current);
	if (current == selected)
	{
		printf("\e[1;7m");
		// printf("\e[41m");
	}
	switch (level) {
		case 0:
			box_width = (SEP_2 - SEP_1) - 3;
			break;
		case 1:
			box_width = (TERM_COLS - SEP_2) - 3;
			break;
		case -1:
			box_width = SEP_1 - 4;
			break;
		default :
			break;
	}
	printf("%.*s", box_width - 1, current->data->d_name);
	found = strcasestr(current->data->d_name, dir_node->search_term);
	if (found != NULL && *dir_node->search_term != 0)
		printf("\e[31m\e[%dG%s", offset + (int)(found - current->data->d_name), dir_node->search_term);
	construct_path(buf, dir_node->dir_name, current->data->d_name);
	if (check_path(copied, buf))
		printf("\e[m\e[33m*\e[m");
	if (check_path(cut, buf))
		printf("\e[m\e[31m*\e[m");
	if (my_strlen(current->data->d_name) > box_width)
		printf("~");
}

void	print_entries(vd_node *dir_node, entry_node *selected, int level)
{
	entry_node			*head;
	entry_node			*current;
	int				offset;
	int 			max = TERM_ROWS - 4;

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
	while (current->pos != (offset + 1))
		current = current->next;
	printf("\e[3;1H");
	while ((current != current->next) && current->pos <= max + offset)
	{
		format_entry(dir_node, current, selected, level);
		printf("\n\e[m");
		current = current->next;
	}
}

void	display_subdirectory(entry_node *selected, char *path)
{
	vd_node				*current;
	entry_node				*selected_sub;

	if (strncmp(path, "/", strlen(path)))
		path = strcat(path, "/");
	path = strcat(path, selected->data->d_name);
	current = get_vd_node(VISITED_DIRS, path);
	current->directory = get_directory(path);
	if (current->directory == NULL)
	{
		return ;
	}
	selected_sub = get_selected(current);
	print_entries(current, selected_sub, 1);
	cleanup_directory(current);
}

void	display_parent(vd_node *dir_node, vd_node *parent)
{
	entry_node	*selected;

	if (strncmp(dir_node->dir_name, "/", strlen(dir_node->dir_name)) == 0)
	{
		clear_parent_box();
		return ;
	}
	parent->directory = get_directory(parent->dir_name);
	selected = get_selected(parent);
	print_entries(parent, selected, -1);
	cleanup_directory(parent);
}


void	preview_text(entry_node *file)
{
	size_t	size = 511;
	char	*line = malloc(size);
	int		line_no = 2;
	FILE	*fp;

	fp = fopen(file->data->d_name, "r");
	if (fp == NULL)
	{
		free(line);
		printf("\e[3;%dH\e[7m%.*s\e[m\n", SEP_2 + 2, (TERM_COLS - SEP_2) - 2, "Cannot preview file");
		return;
	}
	printf("\e[3;1H");
	while (line_no < TERM_ROWS - 2 && (getline(&line, &size, fp)) != -1)
	{
		if (strlen(line) > 0)
			line[strlen(line) - 1] = '\0';
		line = replace_tab(line, size);
		if (!str_printable(line))
		{
			clear_sub_box();
			printf("\e[3;%dH\e[7m%.*s\e[m\n", SEP_2 + 2, (TERM_COLS - SEP_2) - 2, "Cannot preview file");
			break;
		}
		printf("\e[%dG%.*s\n", SEP_2 + 2, (TERM_COLS - SEP_2) - 2, line);
		line_no++;

	}
	free(line);
	fclose(fp);
	// exit(0);
}

void	display_directory(vd_node *dir_node, entry_node *selected, vd_node *parent)
{
	int				size = 500 * sizeof(char);
	char			*cwd_name =  malloc(size);
	int				path_length;

	getcwd(cwd_name, size);
	print_entries(dir_node, selected, 0);
	display_parent(dir_node, parent);
	printf("\e[1;3H[ \e[31;1m%.*s", TERM_COLS - 6, cwd_name);
	if (my_strlen(cwd_name) != 1)
		printf("/");
	if (selected == NULL)
	{
		printf("\e[m ]\e[K");
		path_length = my_strlen(cwd_name) + 8;
		while (path_length++ < TERM_COLS)
			printf("─");
		if (my_strlen(cwd_name) == 1)
			printf("─");
		printf("╮\e[2;%dH┤", TERM_COLS);
		printf("\e[2;%dH┬", SEP_2);
		free(cwd_name);
		return ;
	}
	colour_entry(selected);
	printf("%.*s\e[m ]\e[K", (TERM_COLS - 8 - my_strlen(cwd_name)), selected->data->d_name);
	path_length = my_strlen(cwd_name) + my_strlen(selected->data->d_name) + 8;
	while (path_length++ < TERM_COLS)
		printf("─");
	if (my_strlen(cwd_name) == 1)
		printf("─");
	printf("╮\e[2;%dH┤", TERM_COLS);
	printf("\e[2;%dH┬", SEP_2);
	clear_sub_box();
	if (selected->data->d_type == DT_DIR || selected->data->d_type == DT_LNK)
		display_subdirectory(selected, cwd_name);
	// else if (selected->data->d_type == DT_REG && FLAG_PREVIEW == 1)
	else if (FLAG_PREVIEW == 1 && selected->data->d_type == DT_REG)
		preview_text(selected);
	free(cwd_name);
}
