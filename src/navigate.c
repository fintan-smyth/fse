#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/format.h"
#include <string.h>
#include <sys/ioctl.h>

void	open_dir_in_editor(char *buf)
{
	char	*editor = getenv("EDITOR");

	reset_term_settings();
	if (editor == NULL)
		sprintf(buf, "%s %s", "vim", ".");
	else
		sprintf(buf, "%s %s", editor, ".");
	system(buf);
	set_term_settings();
}

void	open_selected_in_editor(entry_node *selected, char *buf)
{
	char	*editor = getenv("EDITOR");

	reset_term_settings();
	if (editor == NULL)
		sprintf(buf, "%s \"%s\"", "vim", selected->data->d_name);
	else
		sprintf(buf, "%s \"%s\"", editor, selected->data->d_name);
	system(buf);
	set_term_settings();
}

void	toggle_hidden(void)
{
	FLAG_HIDDEN = !FLAG_HIDDEN;
}

void	toggle_preview(void)
{
	FLAG_PREVIEW = !FLAG_PREVIEW;
}

void	execute_shell_cmd(char	*buf)
{
	char	*bufp = buf;
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33mcmd:\e[m ", TERM_ROWS, (TERM_COLS) - 6, "");
	while ((c = getchar()) != '\n')
		*(bufp++) = c;
	if (my_strlen(buf) == 0 || c == 27)
	{
		set_term_settings();
		return ;
	}
	printf("\e[2J\e[H\e[31m###OUTPUT###\e[m\n");
	fflush(stdout);
	system(buf);
	set_term_settings();
	printf("Press any key to continue...");
	c = getchar();
}

void	print_help(void)
{
	fflush(stdout);
	printf("\e[2J\e[H\n\t\e[1;4mBINDS\e[m\n\n");
	printf("\t\e[1mq\e[m\tquit\n");
	printf("\t\e[1mj\e[m\tSelect next\n");
	printf("\t\e[1mk\e[m\tSelect previous\n");
	printf("\t\e[1mh\e[m\tGo to parent directory\n");
	printf("\t\e[1ml\e[m\tOpen\n");
	printf("\t\e[1my\e[m\tCopy selected\n");
	printf("\t\e[1md\e[m\tCut selected\n");
	printf("\t\e[1mp\e[m\tPaste selected\n");
	printf("\t\e[1mc\e[m\tClear copy/cut buffer\n");
	printf("\t\e[1mD\e[m\tDelete selected\n");
	printf("\t\e[1mr\e[m\tRename selected\n");
	printf("\t\e[1mx\e[m\tRun selected executable\n");
	printf("\t\e[1me\e[m\tOpen selected in editor\n");
	printf("\t\e[1mE\e[m\tOpen current directory in editor\n");
	printf("\t\e[1m:\e[m\tExecute shell command\n");
	printf("\t\e[1m/\e[m\tSearch in directory\n");
	printf("\t\e[1mn\e[m\tNext search result\n");
	printf("\t\e[1mN\e[m\tPrevious search result\n");
	printf("\t\e[1mg\e[m\tSelect first entry\n");
	printf("\t\e[1mG\e[m\tSelect last entry\n");
	printf("\t\e[1m[\e[m\tScroll up text preview\n");
	printf("\t\e[1m]\e[m\tScroll down text preview\n");
	printf("\t\e[1mH\e[m\tToggle hidden file visibility\n");
	printf("\t\e[1mP\e[m\tToggle text file preview\n");
	printf("\t\e[1m?\e[m\tDisplay this helpful page!\n");
	getchar();
}

void	select_next(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	entry_node	*children = dir_node->directory->children;

	clear_gutter();
	*preview_offset = 0;
	if ((*selected)->next == (*selected)->next->next)
	{
		(*selected) = children->next;
		free(dir_node->selected_name);
		dir_node->selected_name = strdup((*selected)->data->d_name);
	}
	else
	{
		(*selected) = (*selected)->next;
		free(dir_node->selected_name);
		dir_node->selected_name = strdup((*selected)->data->d_name);
	}
}

void	select_prev(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	clear_gutter();
	*preview_offset = 0;
	if ((*selected)->prev == (*selected)->prev->prev)
	{
		while ((*selected)->next != (*selected)->next->next)
			(*selected) = (*selected)->next;
	}
	else
		(*selected) = (*selected)->prev;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	select_next_search_result(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	entry_node	*children = dir_node->directory->children;

	if (*dir_node->search_term != 0)
	{
		*preview_offset = 0;
		do {
			*selected = (*selected)->next;
			if (*selected == (*selected)->next)
				*selected = children->next;
			free(dir_node->selected_name);
			dir_node->selected_name = strdup((*selected)->data->d_name);
		}
		while (strcasestr((*selected)->data->d_name, dir_node->search_term) == NULL);
	}
}

void	select_prev_search_result(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	if (*dir_node->search_term != 0)
	{
		*preview_offset = 0;
		do {
			*selected = (*selected)->prev;
			if (*selected == (*selected)->prev)
			{
				while ((*selected)->next != (*selected)->next->next)
					*selected = (*selected)->next;
			}
			free(dir_node->selected_name); 
			dir_node->selected_name = strdup((*selected)->data->d_name);
		}
		while (strcasestr((*selected)->data->d_name, dir_node->search_term) == NULL);
	}
}

void	open_selected(entry_node *selected, char *buf)
{
	char	*pager = getenv("PAGER");

	if (selected->data->d_type == DT_DIR)
	{
		chdir(selected->data->d_name);
		return ;
	}
	else if (selected->data->d_type == DT_LNK)
	{
		if (readlink(selected->data->d_name, buf, 500) != -1)
		{
			chdir(buf);
			return ;
		}
	}
	else if (selected->data->d_type == DT_REG)
	{
		char	*ext_buf;

		if ((ext_buf = get_extension(selected->data->d_name)) != NULL)
	 	{
			if (strcmp(ext_buf, "mp4") == 0
					|| strcmp(ext_buf, "mkv") == 0
				)
			{
				sprintf(buf, "%s --no-terminal \"%s\" &", "mpv", selected->data->d_name);
				system(buf);
				free(ext_buf);
				return ;
			}
			free(ext_buf);
		}
		if (pager == NULL)
			sprintf(buf, "%s \"%s\"", "less", selected->data->d_name);
		else if (strncmp(pager, "bat", 3) == 0)
			sprintf(buf, "%s \"%s\"", "bat --paging=always", selected->data->d_name);
		else
			sprintf(buf, "%s \"%s\"", pager, selected->data->d_name);
		system(buf);
		return ;
	}
}

void	yank_selected(vd_node *dir_node, entry_node *selected, char *buf)
{
	construct_path(buf, dir_node->dir_name, selected->data->d_name);
	if (check_path(copied, buf) == 0)
	{
		insert_path_node(buf, copied);
		if (check_path(cut, buf))
			delete_path(buf, cut);
	}
	else {
		delete_path(buf, copied);
		clear_main_box();
	}
}

void	cut_selected(vd_node *dir_node, entry_node *selected, char *buf)
{
	construct_path(buf, dir_node->dir_name, selected->data->d_name);
	if (check_path(cut, buf) == 0)
	{
		insert_path_node(buf, cut);
		if (check_path(copied, buf))
			delete_path(buf, copied);
	}
	else {
		delete_path(buf, cut);
		clear_main_box();
	}
}

void	run_executable(entry_node *selected, char *buf)
{
	reset_term_settings();
	printf("\e[2J\e[H");
	fflush(stdout);
	if (selected->data->d_type == DT_REG && is_executable(selected->data->d_name))
	{
		sprintf(buf, "./\"%s\"", selected->data->d_name);
		system(buf);
		set_term_settings();
		printf("Press any key to continue...");
		getchar();
	}
	set_term_settings();
}

int	search_in_dir(vd_node *dir_node, entry_node **selected, entry_node **search_result)
{
	char 	*searchp;
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33msearch:\e[m ", TERM_ROWS, (TERM_COLS) - 6, "");
	memset(dir_node->search_term, 0, 255);
	searchp = dir_node->search_term;
	while ((c = getchar()) != '\n')
		*searchp++ = c;
	*searchp++ = 0;
	set_term_settings();
	if ((*search_result = get_search_match(dir_node)) == NULL)
		return (1);
	*selected = *search_result;
	draw_box();
	clear_gutter();
	printf("\e[%d;3H[ \e[33msearch:\e[m %.*s ]", TERM_ROWS, (SEP_2) - 6, dir_node->search_term);
	return (0);
}

void	goto_first_entry(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	entry_node *children = dir_node->directory->children;

	*preview_offset = 0;
	clear_gutter();
	*selected = children->next;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	goto_last_entry(vd_node *dir_node, entry_node **selected, int *preview_offset)
{
	*preview_offset = 0;
	clear_gutter();
	while ((*selected)->next != (*selected)->next->next)
		*selected = (*selected)->next;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	delete_selected(vd_node *dir_node, entry_node **selected, char *buf)
{
	char	c;
	int 	len = strlen(dir_node->dir_name);

	memcpy(buf, dir_node->dir_name, len);
	if (strncmp(buf, "/", strlen(buf)))
		strcat(buf, "/");
	strcat(buf, (*selected)->data->d_name);
	clear_gutter();
	printf("\e[%d;3H[ \e[1;33mdelete selected file? [y/N] : \e[m%.*s ]",
		TERM_ROWS, TERM_COLS - 38, (*selected)->data->d_name);
	if ((c = getchar()) == 'y')
	{
		if ((remove(buf)) != 0)
			;
		else if ((*selected)->next == (*selected)->next->next)
		{
			if ((*selected)->prev == (*selected)->prev->prev)
			{
				free(dir_node->selected_name);
				dir_node->selected_name = strdup("");

			}
			else
			{
				(*selected) = (*selected)->prev;
				free(dir_node->selected_name);
				dir_node->selected_name = strdup((*selected)->data->d_name);
			}
		}
		else
		{
			(*selected) = (*selected)->next;
			free(dir_node->selected_name);
			dir_node->selected_name = strdup((*selected)->data->d_name);
		}
	}
}

void	rename_file(vd_node *dir_node, entry_node *selected, char *buf)
{
	char	*bufp = buf;
	char	command_buf[500];
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33mrename:\e[m ", TERM_ROWS, (TERM_COLS) - 6, "");
	while ((c = getchar()) != '\n')
		*(bufp++) = c;
	set_term_settings();
	if (my_strlen(buf) == 0 || c == 27 || str_printable(buf) == 0)
		return ;
	if (check_file_exists(dir_node, buf))
	{
		draw_box();
		display_directory(dir_node, get_selected(dir_node), get_parent(dir_node), 0);
		clear_gutter();
		printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, buf);
		if ((c = getchar()) != 'y')
			return ;
	}
	set_term_settings();
	sprintf(command_buf, "mv \"%s\" \"%s\"", selected->data->d_name, buf);
	if (system(command_buf) == 0)
	{
		free(dir_node->selected_name);
		dir_node->selected_name = strdup(buf);
	}
}

int	navigate(vd_node *dir_node)
{
	char				buf[500];
	char				c;
	int					preview_offset = 0;
	vd_node				*parent;
	entry_node			*selected;
	entry_node			*search_result;

	parent = get_parent(dir_node);
	selected = get_selected(dir_node);
	set_winsize();
	draw_box();
	clear_gutter();
	display_directory(dir_node, selected, parent, preview_offset);
	while ((c = getchar()) != 'q')
	{
		memset(buf, 0, 500 * sizeof(char));
		switch (c) {
			case ('h'):
				chdir("..");
				cleanup_directory(dir_node);
				return (0);
			case ('E'):
				open_dir_in_editor(buf);
				cleanup_directory(dir_node);
				return (0);
			case ('H'):
				toggle_hidden();
				dir_node->offset = 0;
				cleanup_directory(dir_node);
				return (0);
			case ('P'):
				toggle_preview();
				cleanup_directory(dir_node);
				return (0);
			case (':'):
				execute_shell_cmd(buf);
				cleanup_directory(dir_node);
				return (0);
			case ('?'):
				print_help();
				cleanup_directory(dir_node);
				return (0);
			case ('p'):
				clear_gutter();
				paste(dir_node);
				cleanup_directory(dir_node);
				return (0);
			case ('c'):
				clear_path_list(cut);
				cleanup_directory(dir_node);
				return (0);
		}
		if (selected == NULL)
			continue;
		switch (c) {
			case ('j'):
				select_next(dir_node, &selected, &preview_offset);
				break;
			case ('k'):
				select_prev(dir_node, &selected, &preview_offset);
				break;
			case ('n'):
				select_next_search_result(dir_node, &selected, &preview_offset);
				break;
			case ('N'):
				select_prev_search_result(dir_node, &selected, &preview_offset);
				break;
			case ('l'):
				open_selected(selected, buf);
				cleanup_directory(dir_node);
				return (0);
			case ('e'):
				open_selected_in_editor(selected, buf);
				cleanup_directory(dir_node);
				return (0);
			case ('y'):
				yank_selected(dir_node, selected, buf);
				break;
			case ('d'):
				cut_selected(dir_node, selected, buf);
				break;
			case ('x'):
				run_executable(selected, buf);
				cleanup_directory(dir_node);
				return (0);
			case ('/'):
				if (search_in_dir(dir_node, &selected, &search_result) == 1)
				{
					cleanup_directory(dir_node);
					return (0);
				}
				break;
			case (']'):
				if ((selected->lines - preview_offset) <= TERM_ROWS - 4)
					continue;
				preview_offset++;
				break;
			case ('['):
				if (preview_offset <= 0)
					continue;
				preview_offset--;
				break;
			case ('g'):
				goto_first_entry(dir_node, &selected, &preview_offset);
				break;
			case ('G'):
				goto_last_entry(dir_node, &selected, &preview_offset);
				break;
			case ('D'):
				delete_selected(dir_node, &selected, buf);
				cleanup_directory(dir_node);
				return (0);
			case ('r'):
				rename_file(dir_node, selected, buf);
				cleanup_directory(dir_node);
				return (0);
		}
		display_directory(dir_node, selected, parent, preview_offset);
	}
	cleanup_directory(dir_node);
	return (1);
}
