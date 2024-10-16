#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/format.h"

int	navigate(vd_node *dir_node)
{
	char				*buf = malloc(500 * sizeof(char));
	char				*editor = getenv("EDITOR");
	char				*pager = getenv("PAGER");
	entry_node			*children;
	vd_node				*parent;
	entry_node			*selected;
	char				c;

	children = dir_node->directory->children;
	parent = get_parent(dir_node);
	selected = get_selected(dir_node);
	set_winsize();
	draw_box();
	display_directory(dir_node, selected, parent);
	while ((c = getchar()) != 'q')
	{
		memset(buf, 0, 500 * sizeof(char));
		if (c == 'h')
		{
			chdir("..");
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == 'E')
		{
			if (editor == NULL)
				sprintf(buf, "%s %s", "vim", ".");
			else
				sprintf(buf, "%s %s", editor, ".");
			system(buf);
			set_term_settings();
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == 'H')
		{
			FLAG_HIDDEN = !FLAG_HIDDEN;
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == 'P')
		{
			FLAG_PREVIEW = !FLAG_PREVIEW;
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == ':')
		{
			reset_term_settings();
			printf("\e[%d;3H[%*s]\e[4G \e[33mcmd:\e[m ", TERM_ROWS, (SEP_2) - 6, "");
			char *bufp = buf;
			while ((c = getchar()) != '\n')
				*(bufp++) = c;
			printf("\e[2J\e[H\e[31m###OUTPUT###\e[m\n");
			fflush(stdout);
			system(buf);
			set_term_settings();
			c = getchar();
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == 'R')
		{
			// refresh_dir(dir_node);
			cleanup_directory(dir_node->directory);
			free(buf);
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
			printf("\t\e[1mD\e[m\tDelete selected\n");
			printf("\t\e[1me\e[m\tOpen selected in editor\n");
			printf("\t\e[1mE\e[m\tOpen current directory in editor\n");
			printf("\t\e[1m:\e[m\tExecute shell command\n");
			printf("\t\e[1mH\e[m\tToggle hidden file visibility\n");
			printf("\t\e[1mP\e[m\tToggle text file preview\n");
			printf("\t\e[1mR\e[m\tReload directory\n");
			printf("\t\e[1m?\e[m\tDisplay this helpful page!\n");
			getchar();
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		if (selected == NULL)
			continue;
		else if (c == 'j')
		{
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
		else if (c == 'l')
		{
			if (selected->data->d_type == DT_DIR)
			{
				chdir(selected->data->d_name);
				cleanup_directory(dir_node->directory);
				free(buf);
				return (0);
			}
			else if (selected->data->d_type == DT_LNK)
			{
				if (readlink(selected->data->d_name, buf, 500) != -1)
				{
					chdir(buf);
					cleanup_directory(dir_node->directory);
					free(buf);
					return (0);
				}
			}
			else if (selected->data->d_type == DT_REG)
			{
				char	ext_buf[10];
				if (get_extension(ext_buf, selected->data->d_name) == NULL)
			 	{
					if (pager == NULL)
						sprintf(buf, "%s %s", "less", selected->data->d_name);
					else if (strncmp(pager, "bat", 3) == 0)
						sprintf(buf, "%s %s", "bat --paging=always", selected->data->d_name);
					else
						sprintf(buf, "%s %s", pager, selected->data->d_name);
					system(buf);
				}
				else if (strcmp(ext_buf, "mp4") == 0
						|| strcmp(ext_buf, "mkv") == 0
						// || strcmp(ext_buf, "") == 0
					)
				{
					sprintf(buf, "%s --no-terminal \"%s\" &", "mpv", selected->data->d_name);
					system(buf);
				}
				cleanup_directory(dir_node->directory);
				free(buf);
				return (0);
			}
		}
		else if (c == 'e')
		{
			if (editor == NULL)
				sprintf(buf, "%s %s", "vim", selected->data->d_name);
			else
				sprintf(buf, "%s %s", editor, selected->data->d_name);
			system(buf);
			set_term_settings();
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		else if (c == 'D')
		{
			if (selected->data->d_type != DT_SOCK)
			{
				strncpy(buf, dir_node->dir_name, strlen(dir_node->dir_name));
				if (strncmp(buf, "/", strlen(buf)))
					strcat(buf, "/");
				strcat(buf, selected->data->d_name);
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
			cleanup_directory(dir_node->directory);
			free(buf);
			return (0);
		}
		display_directory(dir_node, selected, parent);
	}
	cleanup_directory(dir_node->directory);
	free(buf);
	return (1);
}

int	main(void)
{
	int				size = 500 * sizeof(char);
	char			*cwd_name =  malloc(size);
	vd_node			*current;
	
	VISITED_DIRS = init_visited();
	set_term_settings();
	atexit(reset_term_settings);
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
	free_visited(VISITED_DIRS);
	free(cwd_name);
}
