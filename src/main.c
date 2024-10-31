#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/format.h"
#include <dirent.h>
#include <stdio.h>



int	navigate(vd_node *dir_node)
{
	char				buf[500];
	char				*editor = getenv("EDITOR");
	char				*pager = getenv("PAGER");
	char 				*searchp;
	int					preview_start = 0;
	entry_node			*children;
	vd_node				*parent;
	entry_node			*selected;
	entry_node			*search_result;
	unsigned char				c;

	children = dir_node->directory->children;
	parent = get_parent(dir_node);
	selected = get_selected(dir_node);
	set_winsize();
	draw_box();
	clear_gutter();
	display_directory(dir_node, selected, parent, preview_start);
	while ((c = getchar()) != 'q')
	{
		memset(buf, 0, 500 * sizeof(char));
		if (c == 'h')
		{
			chdir("..");
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'E')
		{
			reset_term_settings();
			if (editor == NULL)
				sprintf(buf, "%s %s", "vim", ".");
			else
				sprintf(buf, "%s %s", editor, ".");
			system(buf);
			set_term_settings();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'H')
		{
			FLAG_HIDDEN = !FLAG_HIDDEN;
			dir_node->offset = 0;
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'P')
		{
			FLAG_PREVIEW = !FLAG_PREVIEW;
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == ':')
		{
			reset_term_settings();
			printf("\e[%d;3H[%*s]\e[4G \e[33mcmd:\e[m ", TERM_ROWS, (TERM_COLS) - 6, "");
			char *bufp = buf;
			while ((c = getchar()) != '\n')
				*(bufp++) = c;
			if (my_strlen(buf) == 0 || c == 27)
			{
				set_term_settings();
				cleanup_directory(dir_node);
				return (0);
			}
			printf("\e[2J\e[H\e[31m###OUTPUT###\e[m\n");
			fflush(stdout);
			system(buf);
			set_term_settings();
			printf("Press any key to continue...");
			c = getchar();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'R')
		{
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == '?')
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
			printf("\t\e[1mx\e[m\tRun selected executable\n");
			printf("\t\e[1me\e[m\tOpen selected in editor\n");
			printf("\t\e[1mE\e[m\tOpen current directory in editor\n");
			printf("\t\e[1m:\e[m\tExecute shell command\n");
			printf("\t\e[1m/\e[m\tSearch in directory\n");
			printf("\t\e[1mn\e[m\tNext search result\n");
			printf("\t\e[1mN\e[m\tPrevious search result\n");
			printf("\t\e[1m[\e[m\tScroll up text preview\n");
			printf("\t\e[1m]\e[m\tScroll down text preview\n");
			printf("\t\e[1mH\e[m\tToggle hidden file visibility\n");
			printf("\t\e[1mP\e[m\tToggle text file preview\n");
			printf("\t\e[1mR\e[m\tReload directory\n");
			printf("\t\e[1m?\e[m\tDisplay this helpful page!\n");
			getchar();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'p')
		{
			clear_gutter();
			paste(dir_node);
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'c')
		{
			clear_path_list(copied);
			clear_path_list(cut);
			clear_path_list(highlighted);
			cleanup_directory(dir_node);
			return (0);
		}
		if (selected == NULL)
			continue;
		else if (c == 'j')
		{
			clear_gutter();
			preview_start = 0;
			if (selected->next == selected->next->next)
			{
				selected = children->next;
				free(dir_node->selected_name); 
				dir_node->selected_name = strdup(selected->data->d_name);
			}
			else
			{
				selected = selected->next;
				free(dir_node->selected_name); 
				dir_node->selected_name = strdup(selected->data->d_name);
			}
		}
		else if(c == 'k')
		{
			clear_gutter();
			preview_start = 0;
			if (selected->prev == selected->prev->prev)
			{
				while (selected->next != selected->next->next)
					selected = selected->next;
			}
			else
				selected = selected->prev;
			free(dir_node->selected_name); 
			dir_node->selected_name = strdup(selected->data->d_name);
		}
		else if (c == 'n')
		{
			if (*dir_node->search_term != 0)
			{
				do {
					selected = selected->next;
					if (selected == selected->next)
						selected = children->next;
					free(dir_node->selected_name); 
					dir_node->selected_name = strdup(selected->data->d_name);
				}
				while (strcasestr(selected->data->d_name, dir_node->search_term) == NULL);
			}
		}
		else if (c == 'N')
		{
			if (*dir_node->search_term != 0)
			{
				do {
					selected = selected->prev;
					if (selected == selected->prev)
					{
						while (selected->next != selected->next->next)
							selected = selected->next;
					}
					free(dir_node->selected_name); 
					dir_node->selected_name = strdup(selected->data->d_name);
				}
				while (strcasestr(selected->data->d_name, dir_node->search_term) == NULL);
			}
		}
		else if (c == 'l')
		{
			if (selected->data->d_type == DT_DIR)
			{
				chdir(selected->data->d_name);
				cleanup_directory(dir_node);
				return (0);
			}
			else if (selected->data->d_type == DT_LNK)
			{
				if (readlink(selected->data->d_name, buf, 500) != -1)
				{
					chdir(buf);
					cleanup_directory(dir_node);
					return (0);
				}
			}
			else if (selected->data->d_type == DT_REG)
			{
				char	*ext_buf;
				if ((ext_buf = get_extension(selected->data->d_name)) != NULL)
			 	{
					if (strcmp(ext_buf, "mp4") == 0
							|| strcmp(ext_buf, "mkv") == 0
							// || strcmp(ext_buf, "") == 0
						)
					{
						sprintf(buf, "%s --no-terminal \"%s\" &", "mpv", selected->data->d_name);
						system(buf);
						cleanup_directory(dir_node);
						free(ext_buf);
						return (0);
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
				cleanup_directory(dir_node);
				return (0);
			}
		}
		else if (c == 'e')
		{
			reset_term_settings();
			if (editor == NULL)
				sprintf(buf, "%s \"%s\"", "vim", selected->data->d_name);
			else
				sprintf(buf, "%s \"%s\"", editor, selected->data->d_name);
			system(buf);
			set_term_settings();
			cleanup_directory(dir_node);
			return (0);
		}
		else if (c == 'y')
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
		else if (c == 'd')
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
		else if (c == 'x')
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
			cleanup_directory(dir_node);
			return (0);
		}
		// else if (c == ' ')
		// {
		// 	construct_path(buf, dir_node->dir_name, selected->data->d_name);
		// 	if (check_path(highlighted, buf) == 0)
		// 	{
		// 		insert_path_node(buf, cut);
		// 		if (check_path(copied, buf))
		// 			delete_path(buf, copied);
		// 	}
		// 	else {
		// 		delete_path(buf, cut);
		// 		clear_main_box();
		// 	}
		// }
		else if (c == '/')
		{
			reset_term_settings();
			printf("\e[%d;3H[%*s]\e[4G \e[33msearch:\e[m ", TERM_ROWS, (TERM_COLS) - 6, "");
			memset(dir_node->search_term, 0, 255);
			searchp = dir_node->search_term;
			while ((c = getchar()) != '\n')
				*searchp++ = c;
			*searchp++ = 0;
			set_term_settings();
			if ((search_result = get_search_match(dir_node)) != NULL)
				selected = search_result;
			else
			{
				cleanup_directory(dir_node);
				return (0);
			}
			draw_box();
			clear_gutter();
			printf("\e[%d;3H[ \e[33msearch:\e[m %.*s ]", TERM_ROWS, (SEP_2) - 6, dir_node->search_term);
		}
		else if (c == ']')
		{
			if ((selected->lines - preview_start) <= TERM_ROWS - 4)
				continue;
			preview_start++;
		}
		else if (c == '[')
		{
			if (preview_start <= 0)
				continue;
			preview_start--;
		}
		else if (c == 'w')
		{
			is_binary(selected->data->d_name);
			cleanup_directory(dir_node);
			exit(0);
		}
		else if (c == 'D')
		{
			if (selected->data->d_type != DT_SOCK)
			{
				int len = strlen(dir_node->dir_name);
				memcpy(buf, dir_node->dir_name, len);
				if (strncmp(buf, "/", strlen(buf)))
					strcat(buf, "/");
				strcat(buf, selected->data->d_name);
				clear_gutter();
				printf("\e[%d;3H[ \e[1;33mdelete selected file? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, selected->data->d_name);
				if ((c = getchar()) == 'y')
				{
					if ((remove(buf)) != 0)
					{
						;
					}
					else if (selected->next == selected->next->next)
					{
						if (selected->prev == selected->prev->prev)
						{
							free(dir_node->selected_name); 
							dir_node->selected_name = strdup("");	

						}
						else
						{
							selected = selected->prev;
							free(dir_node->selected_name); 
							dir_node->selected_name = strdup(selected->data->d_name);	
						}
					}
					else
					{
						selected = selected->next;
						free(dir_node->selected_name); 
						dir_node->selected_name = strdup(selected->data->d_name);	
					}
				}
			}
			cleanup_directory(dir_node);
			return (0);
		}
		display_directory(dir_node, selected, parent, preview_start);
	}
	cleanup_directory(dir_node);
	return (1);
}

void	exit_cleanup()
{
	reset_term_settings();
	free_visited(VISITED_DIRS);
	free_path_list(copied);
	free_path_list(cut);
	free_path_list(highlighted);
}

int	main(void)
{
	int				size = 500 * sizeof(char);
	char			cwd_name[500];
	vd_node			*current;
	
	VISITED_DIRS = init_visited();
	copied = init_path_list();
	cut = init_path_list();
	highlighted = init_path_list();
	set_term_settings();
	atexit(exit_cleanup);
	getcwd(cwd_name, size);
	current = vd_insert(VISITED_DIRS, cwd_name);
	current->directory = get_directory(cwd_name);
	while (navigate(current) == 0)
	{
		getcwd(cwd_name, size);
		current = get_vd_node(VISITED_DIRS, cwd_name);
		current->directory = get_directory(cwd_name);
	}
	printf("\e[2J\e[H");
}
