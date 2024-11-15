#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/format.h"
#include "headers/env.h"
#include <stdio.h>

void	open_dir_in_editor(char *buf)
// Opens cwd in $EDITOR.
// Uses vim if $EDITOR is unset.
// Args:
//  - buf:	char array used to construct shell command
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
// Opens selected entry in $EDITOR.
// Uses vim if $EDITOR is unset.
// Args:
//  - selected:	pointer to node of selected entry
//  - buf:		char array used to construct shell command
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
// Toggles showing hidden files.
{
	env.FLAGS ^= F_HIDDEN;
}

void	toggle_preview(void)
// Toggles previewing parent directory.
{
	env.FLAGS ^= F_PREVIEW;
}

void	toggle_reverse_sort(void)
// Reverses order of  the sort.
{
	env.FLAGS ^= F_REVERSE;
}

void	pick_sort_method(void)
// Spawns a popup prompting the user to pick sorting options.
{
	char	c;

	spawn_popup(5);
	printf("\e[%d;3H\e[1mKeys\tCommand\e[m", env.TERM_ROWS - 6);
	printf("\e[%d;3H%ca\t(%s) Sort alphabetical", env.TERM_ROWS - 5, binds.PICK_SORT, (env.SORT == SORT_ALPHA) ? "\e[1;32m*\e[m" : " ");
	printf("\e[%d;3H%cf\t(%s) Sort by filesize", env.TERM_ROWS - 4, binds.PICK_SORT, (env.SORT == SORT_SIZE) ? "\e[1;32m*\e[m" : " ");
	printf("\e[%d;3H%ct\t(%s) Sort by mtime", env.TERM_ROWS - 3, binds.PICK_SORT, (env.SORT == SORT_MTIME) ? "\e[1;32m*\e[m" : " ");
	printf("\e[%d;3H%cr\t[%s] Reverse sort order", env.TERM_ROWS - 2, binds.PICK_SORT, (env.FLAGS & F_REVERSE) ? "\e[1;31mX\e[m" : " ");
	c = getchar();
	if (c == 'a')
		env.SORT = SORT_ALPHA;
	else if (c == 'f')
		env.SORT = SORT_SIZE;
	else if (c == 't')
		env.SORT = SORT_MTIME;
	else if (c == 'r')
		toggle_reverse_sort();
}

void	insert_entry(vd_node *dir_node, char *buf)
// Prompts the user to insert a new entry into the directory.
// Args:
//  - buf:	char array used to construct shell command
{
	char	*bufp = buf;
	char	command_buf[500];
	char	c;
	int		len;
	int		is_dir = 0;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33minsert:\e[m ", env.TERM_ROWS, (env.TERM_COLS) - 6, "");
	while ((c = getchar()) != '\n')
		*(bufp++) = c;
	set_term_settings();
	len = my_strlen(buf);
	if (len == 0 || c == 27 || str_printable(buf) == 0)
		return ;
	if (buf[len - 1] == '/')
	{
		buf[len - 1] = 0;
		is_dir = 1;
	}
	if (check_file_exists(dir_node, buf))
		return ;
	if (is_dir)
	{
		sprintf(command_buf, "mkdir -p \"%s\"", buf);
		system(command_buf);
		return ;
	}
	sprintf(command_buf, "touch \"%s\"", buf);
	system(command_buf);
	return ;
}

void	execute_shell_cmd(char	*buf)
// Prompts the user to execute a shell command.
// Args:
//  - buf:	char array used to construct shell command
{
	char	*bufp = buf;
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33mcmd:\e[m ", env.TERM_ROWS, (env.TERM_COLS) - 6, "");
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
// Prints a list of the program binds.
{
	fflush(stdout);
	printf("\e[2J\e[H\n\t\e[1;4mBINDS\e[m\n\n");
	printf("\t\e[1m%c\e[m\tQuit\n", binds.QUIT);
	printf("\t\e[1m<0-9>\e[m\tInput number to modify applicable commands\n");
	printf("\t\e[1m%c\e[m\tSelect next\n", binds.SELECT_NEXT);
	printf("\t\e[1m%c\e[m\tSelect previous\n", binds.SELECT_PREV);
	printf("\t\e[1m%c\e[m\tGo to parent directory\n", binds.UPDIR);
	printf("\t\e[1m%c\e[m\tOpen\n", binds.OPEN);
	printf("\t\e[1m%c\e[m\tCopy selected\n", binds.YANK);
	printf("\t\e[1m%c\e[m\tCut selected\n", binds.CUT);
	printf("\t\e[1m%c\e[m\tPaste selected\n", binds.PASTE);
	printf("\t\e[1m%c\e[m\tClear copy/cut buffer\n", binds.CLEAR_BUF);
	printf("\t\e[1m%c\e[m\tDelete selected\n", binds.DELETE);
	printf("\t\e[1m%c\e[m\tRename selected\n", binds.RENAME);
	printf("\t\e[1m%c\e[m\tInsert new entry\n", binds.INSERT);
	printf("\t\e[1m%c\e[m\tRun selected executable\n", binds.EXEC_FILE);
	printf("\t\e[1m%c\e[m\tOpen selected in editor\n", binds.EDIT_FILE);
	printf("\t\e[1m%c\e[m\tOpen current directory in editor\n", binds.EDIT_DIR);
	printf("\t\e[1m%c\e[m\tExecute shell command\n", binds.EXEC_SHELL);
	printf("\t\e[1m%c\e[m\tSearch in directory\n", binds.SEARCH_DIR);
	printf("\t\e[1m%c\e[m\tNext search result\n", binds.SEARCH_NEXT);
	printf("\t\e[1m%c\e[m\tPrevious search result\n", binds.SEARCH_PREV);
	printf("\t\e[1m%c\e[m\tSelect first entry\n", binds.GO_FIRST);
	printf("\t\e[1m%c\e[m\tSelect last entry\n", binds.GO_LAST);
	printf("\t\e[1m%c\e[m\tGo to home directory\n", binds.GO_HOME);
	printf("\t\e[1m%c\e[m\tScroll down text preview\n", binds.PREV_DOWN);
	printf("\t\e[1m%c\e[m\tScroll up text preview\n", binds.PREV_UP);
	printf("\t\e[1m%c\e[m\tToggle hidden file visibility\n", binds.TOGGLE_HIDDEN);
	printf("\t\e[1m%c\e[m\tPick sorting options\n", binds.PICK_SORT);
	printf("\t\e[1m%c\e[m\tToggle parent directory preview\n", binds.TOGGLE_PARENT);
	printf("\t\e[1m%c\e[m\tDisplay this helpful page!\n", binds.HELP);
	getchar();
}

void	select_next(vd_node *dir_node, entry_node **selected, int cmd_count, int *preview_offset)
// Selects the next entry in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - cmd_count			the number of entries to move selection down by
//  - preview_offset	pointer to preview_offset variable in navigate()
{
	int			i = 0;

	if (cmd_count == 0)
		cmd_count = 1;
	clear_gutter();
	*preview_offset = 0;
	while (i < cmd_count && (*selected)->next != (*selected)->next->next)
	{
		(*selected) = (*selected)->next;
		i++;
	}
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	select_prev(vd_node *dir_node, entry_node **selected, int cmd_count, int *preview_offset)
// Selects the previous entry in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - cmd_count			the number of entries to move selection down by
//  - preview_offset	pointer to preview_offset variable in navigate()
{
	int			i = 0;

	if (cmd_count == 0)
		cmd_count = 1;
	clear_gutter();
	*preview_offset = 0;
	while (i < cmd_count && (*selected)->prev != (*selected)->prev->prev)
	{
		(*selected) = (*selected)->prev;
		i++;
	}
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	select_next_search_result(vd_node *dir_node, entry_node **selected, int *preview_offset)
// Selects the next entry matching the search term in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - preview_offset	pointer to preview_offset variable in navigate()
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
// Selects the previous entry matching the search term in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - preview_offset	pointer to preview_offset variable in navigate()
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
// Opens the selected entry in a different manner depending on its type.
// Args:
//  - selected:	pointer to selected entry node
//  - buf:		char array used to construct shell command
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
		printf("\e[2J\e[H");
		fflush(stdout);
		system(buf);
		return ;
	}
}

void	yank_selected(vd_node *dir_node, entry_node *selected, int cmd_count, char *buf)
// Adds selected entry path to 'copied' list.
// Args:
//  - dir_node:	pointer to current directory node
//  - selected:	pointer to selected entry node
//  - buf:		char array used to construct absolute path of file
{
	int	i = 0;

	if (cmd_count == 0)
		cmd_count = 1;
	while (i < cmd_count && selected != selected->next)
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
		selected = selected->next;
		i++;
	}
}

void	cut_selected(vd_node *dir_node, entry_node *selected, int cmd_count, char *buf)
// Adds selected entry path to 'cut' list.
// Args:
//  - dir_node:	pointer to current directory node
//  - selected:	pointer to selected entry node
//  - buf:		char array used to construct absolute path of file
{
	int	i = 0;

	if (cmd_count == 0)
		cmd_count = 1;
	while (i < cmd_count && selected != selected->next)
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
		selected = selected->next;
		i++;
	}
}

void	run_executable(entry_node *selected, char *buf)
// If executable, runs the selected file.
// Args:
//  - selected:	pointer to selected entry node
//  - buf:		char array used to construct shell command
{
	reset_term_settings();
	printf("\e[2J\e[H");
	fflush(stdout);
	if (selected->data->d_type == DT_REG && (selected->attr->st_mode & S_IXUSR))
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
// Prompts the user to enter a string to match entry file names against.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - search_result		pointer to search_result variable in navigate()
// Returns:
//  - 0 if a match is found
//  - 1 if no match is found
{
	char 	*searchp;
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33msearch:\e[m ", env.TERM_ROWS, (env.TERM_COLS) - 6, "");
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
	printf("\e[%d;3H[ \e[33msearch:\e[m %.*s ]", env.TERM_ROWS, (env.SEP_2) - 6, dir_node->search_term);
	return (0);
}

void	goto_first_entry(vd_node *dir_node, entry_node **selected, int *preview_offset)
// Selects the first entry in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - preview_offset	pointer to preview_offset variable in navigate()
{
	entry_node *children = dir_node->directory->children;

	*preview_offset = 0;
	clear_gutter();
	*selected = children->next;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	goto_entry_no(vd_node *dir_node, entry_node **selected, int *preview_offset, int entry_no)
// Selects the entry at a specific position in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - preview_offset	pointer to preview_offset variable in navigate()
//  - entry_no			position of the entry to select
{
	entry_node *children = dir_node->directory->children;

	*preview_offset = 0;
	clear_gutter();
	*selected = children->next;
	while (((*selected)->next != (*selected)->next->next) && entry_no != (*selected)->pos)
		*selected = (*selected)->next;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	goto_last_entry(vd_node *dir_node, entry_node **selected, int *preview_offset)
// Selects the last entry in the current directory.
// Args:
//  - dir_node:			pointer to current directory node
//  - selected:			address of pointer to selected entry
//  - preview_offset	pointer to preview_offset variable in navigate()
{
	*preview_offset = 0;
	clear_gutter();
	while ((*selected)->next != (*selected)->next->next)
		*selected = (*selected)->next;
	free(dir_node->selected_name);
	dir_node->selected_name = strdup((*selected)->data->d_name);
}

void	delete_selected(vd_node *dir_node, entry_node **selected, char *buf)
// Deletes the selected file or empty directory.
// Args:
//  - dir_node:		pointer to current directory node
//  - selected:		address of pointer to selected entry
//  - buf			char array used to construct absolute path
{
	char	c;
	int 	len = strlen(dir_node->dir_name);

	memcpy(buf, dir_node->dir_name, len);
	if (strncmp(buf, "/", strlen(buf)))
		strcat(buf, "/");
	strcat(buf, (*selected)->data->d_name);
	clear_gutter();
	printf("\e[%d;3H[ \e[1;33mdelete selected file? [y/N] : \e[m%.*s ]",
		env.TERM_ROWS, env.TERM_COLS - 38, (*selected)->data->d_name);
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
// Prompts the user to rename the selected entry.
// Args:
//  - dir_node:	pointer to current directory node
//  - selected:	pointer to selected entry node
//  - buf:		char array used to store user input
{
	char	*bufp = buf;
	char	command_buf[500];
	char	c;

	reset_term_settings();
	printf("\e[%d;3H[%*s]\e[4G \e[33mrename:\e[m ", env.TERM_ROWS, (env.TERM_COLS) - 6, "");
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
		printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", env.TERM_ROWS, env.TERM_COLS - 38, buf);
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
// Displays current state of directory and waits for user input.
// Args:
//  - dir_node:	pointer to current directory node
// Returns:
//  - 0 when user wishes to continue file system navigation
//  - 1 when user requests to quit the program
{
	char				buf[500];
	char				c;
	int					preview_offset = 0;
	int					cmd_count;
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
		cmd_count = 0;
		if (c > '0' && c <= '9')
		{
			int	i = 0;

			buf[i++] = c;
			while (is_digit(c = getchar()))
				buf[i++] = c;
			buf[i] = 0;
			cmd_count = my_atoi(buf);
			// printf("\e[2J\e[H%d%c\n", cmd_count, c);
			// exit(0);
		}
		if (c == binds.UPDIR)
		{
			chdir("..");
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.GO_HOME)
		{
			char *home = getenv("HOME");
			chdir(home);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.INSERT)
		{
			insert_entry(dir_node, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.EDIT_DIR)
		{
			open_dir_in_editor(buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.TOGGLE_HIDDEN)
		{
			toggle_hidden();
			dir_node->offset = 0;
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.TOGGLE_PARENT)
		{
			toggle_preview();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.PICK_SORT)
		{
			pick_sort_method();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.EXEC_SHELL)
		{
			execute_shell_cmd(buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.HELP)
		{
			print_help();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.PASTE)
		{
			clear_gutter();
			paste(dir_node);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.CLEAR_BUF)
		{
			clear_path_list(copied);
			clear_path_list(cut);
			cleanup_directory(dir_node);
			return (0);
		}
		if (selected == NULL)
			continue;
		if (c == binds.SELECT_NEXT)
		{
			if (selected->next == selected->next->next)
				continue ;
			select_next(dir_node, &selected, cmd_count, &preview_offset);
		}
		else if (c == binds.SELECT_PREV)
		{
			if (selected->prev == selected->prev->prev)
				continue ;
			select_prev(dir_node, &selected, cmd_count, &preview_offset);
		}
		else if (c == binds.SEARCH_NEXT)
			select_next_search_result(dir_node, &selected, &preview_offset);
		else if (c == binds.SEARCH_PREV)
			select_prev_search_result(dir_node, &selected, &preview_offset);
		else if (c == binds.OPEN)
		{
			open_selected(selected, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.EDIT_FILE)
		{
			open_selected_in_editor(selected, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.YANK)
			yank_selected(dir_node, selected, cmd_count, buf);
		else if (c == binds.CUT)
			cut_selected(dir_node, selected, cmd_count, buf);
		else if (c == binds.EXEC_FILE)
		{
			run_executable(selected, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.SEARCH_DIR)
		{
			if (search_in_dir(dir_node, &selected, &search_result) == 1)
			{
				cleanup_directory(dir_node);
				return (0);
			}
		}
		else if (c == binds.PREV_DOWN)
		{
			if ((selected->lines - preview_offset) <= env.TERM_ROWS - 4)
				continue;
			preview_offset++;
		}
		else if (c == binds.PREV_UP)
		{
			if (preview_offset <= 0)
				continue;
			preview_offset--;
		}
		else if (c == binds.GO_FIRST)
		{
			if (cmd_count == 0)
				goto_first_entry(dir_node, &selected, &preview_offset);
			else
				goto_entry_no(dir_node, &selected, &preview_offset, cmd_count);
		}
		else if (c == binds.GO_LAST)
		{
			if (cmd_count == 0)
				goto_last_entry(dir_node, &selected, &preview_offset);
			else
				goto_entry_no(dir_node, &selected, &preview_offset, cmd_count);
		}
		else if (c == binds.DELETE)
		{
			delete_selected(dir_node, &selected, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == binds.RENAME)
		{
			rename_file(dir_node, selected, buf);
			cleanup_directory(dir_node);
			return (0);
		}
		else
			continue;
		display_directory(dir_node, selected, parent, preview_offset);
	}
	cleanup_directory(dir_node);
	return (1);
}
