#include "headers/trash.h"
#include "headers/env.h"
#include "headers/format.h"
#include "headers/structs.h"
#include "headers/utils.h"
#include <dirent.h>

trash_node	*trash_list;

trash_node *init_trash_list(void)
// Initialises a doubly linked list of nodes containing information
// about the user's trash folder.
// Returns:
//  - Pointer to head of the trash list
{
	trash_node *head;
	trash_node *tail;

	head = (trash_node *)malloc(sizeof(*head));
	tail = (trash_node *)malloc(sizeof(*tail));
	head->next = tail;
	head->prev = head;
	head->name = NULL;
	head->old_name = NULL;
	head->old_location = NULL;
	head->pos = 0;
	head->type = 0;
	tail->next = tail;
	tail->prev = head;
	tail->name = NULL;
	tail->old_name = NULL;
	tail->old_location = NULL;
	tail->pos = -5;
	tail->type = 0;
	return (head);
}

void	delete_next_trash_node(trash_node *t)
// Deletes the next node from the given trash list.
// Args:
//  - t:	pointer to the node before the node to be deleted
{
	trash_node *temp;
	temp = t->next;
	t->next = t->next->next;
	t->next->prev = t;
	free(temp->name);
	free(temp->old_name);
	free(temp->old_location);
	free(temp);
}

void	delete_selected_trash_node(trash_node *head, trash_node *to_delete)
// Deletes the given node from a given trash list.
// Args:
//  - head:			pointer to the head of the trash list
//  - to_delete:	pointer to the node to be deleted
{
	trash_node	*current;

	current = head;
	while (current->next != to_delete && current != current->next)
		current = current->next;
	if (current == current->next)
		return ;
	delete_next_trash_node(current);
	return ;
}

trash_node *insert_trash_node(char *name, char *old_name, char *old_location, int type, trash_node *head)
{
	trash_node	*new;
	trash_node	*current;

	new = (trash_node *)malloc(sizeof(*new));
	current = head;
	while (current->next != current->next->next)
		current = current->next;
	new->next = current->next;
	new->prev = current;
	new->name = strdup(name);
	new->old_name = strdup(old_name);
	new->old_location = strdup(old_location);
	new->pos = current->pos + 1;
	new->type = type;
	current->next = new;
	new->next->prev = new;
	return (new);
}

trash_node *insert_selected_trash(vd_node *dir_node, entry_node *selected, trash_node *head)
{
	trash_node	*new;
	char		buf[500];

	sprintf(buf, "%s_%d", selected->data->d_name, (int) selected->attr->st_ctime);
	new = insert_trash_node(buf, selected->data->d_name, dir_node->dir_name, selected->data->d_type, head);
	return (new);
}

void	free_trash(trash_node *head)
// Frees a list of trash nodes.
// Args:
//  - head:	pointer to the head of the trash list.
{
	while (head->next != head->next->next)
		delete_next_trash_node(head);
	free(head->next);
	free(head);
}

void	write_trash_file(void)
{
	trash_node	*current;
	FILE			*fptr;
	char			*home = getenv("HOME");
	char			path[140];

	strncpy(path, home, 100);
	strcat(path, "/.local/share/fse/.trash_index");
	fptr = fopen(path, "w");
	if (fptr == NULL)
	{
		printf("\e[2J\e[HError!\n");
		exit(0);
	}
	current = trash_list->next;
	while (current != current->next)
	{
		fprintf(fptr, "%s\t%s\t%s\t%d\n", current->name, current->old_name, current->old_location, current->type);
		current = current->next;
	}
	fclose(fptr);
}

void	load_trash_file(void)
{
	FILE			*fptr;
	char			*home = getenv("HOME");
	char			path[140];
	char			name[200];
	char			old_name[200];
	char			old_location[200];
	char			type_buf[200];
	int				type;
	char			*line;
	size_t			size = 500;
	int				i;
	int				j;

	strncpy(path, home, 100);
	strcat(path, "/.local/share/fse/.trash_index");
	fptr = fopen(path, "r");
	if (fptr == NULL)
	{
		return ;
	}
	line = malloc(size);
	while (getline(&line, &size, fptr) != -1)
	{
		i = 0;
		j = 0;
		while (line[i] != '\t')
			name[j++] = line[i++];
		name[j] = 0;
		j = 0;
		i++;
		while (line[i] != '\t')
			old_name[j++] = line[i++];
		old_name[j] = 0;
		j = 0;
		i++;
		while (line[i] != '\t')
			old_location[j++] = line[i++];
		old_location[j] = 0;
		j = 0;
		i++;
		while (line[i] != '\n')
			type_buf[j++] = line[i++];
		type_buf[j] = 0;
		type = my_atoi(type_buf);
		insert_trash_node(name, old_name, old_location, type, trash_list);
	}
	fclose(fptr);
	free(line);
}

int	count_trash_nodes(trash_node *head)
// Count the number of nodes in the trash list.
// Args:
//  - head:	pointer to the head of the trash list
// Returns:
//  - The number of trash nodes in the list
{
	trash_node	*current;

	current = head;
	while (current->next != current->next->next)
		current = current->next;
	return (current->pos);
}

void	number_trash_nodes(trash_node *head)
// Numbers the position of each trash node in the list.
// Args:
//  - head:	pointer to the head of the trash list
{
	trash_node	*current;
	int				i;

	current = head->next;
	i = 1;
	while (current != current->next)
	{
		current->pos = i++;
		current = current->next;
	}
}

int	longest_trash_name(trash_node *head)
// Returns the length of the longest trash node name.
// Args:
//  - head:	pointer to the head of the trash list
// Returns:
//  - Length of the longest trash node name
//  - 0 if there are no trash nodes
{
	trash_node	*current;
	int				len = 0;

	current = head->next;
	while (current != current->next)
	{
		if (my_strlen(current->old_name) > len)
			len = my_strlen(current->old_name);
		current = current->next;
	}
	return (len);
}

void	print_trash_no(trash_node *selected)
{
	int	len;
	int	pos;
	int	total = count_trash_nodes(trash_list);
	int	entries_digits;

	if (selected == NULL)
		pos = 0;
	else
		pos = selected->pos;
	entries_digits = count_digits(total);
	len = 6 + (2 * entries_digits);
	printf("\e[%d;%dH[ \e[1;33m%0*d/%d\e[m ]",
		env.TERM_ROWS, (env.TERM_COLS - len), entries_digits, pos, total);
}

void	display_trash(trash_node *head, trash_node *selected, int lines, int *offset)
{
	trash_node	*current;
	int				start_line = env.TERM_ROWS - (lines + 2);
	int				longest_name = longest_trash_name(head);

	current = head->next;
	print_trash_no(selected);
	if (selected == NULL)
	{
		clear_popup(lines);
		printf("\e[%d;3H\e[1;39;7m<Empty>\e[m", start_line + 1);
		return ;
	}
	if (selected->pos > lines + *offset)
	{
		clear_popup(lines);
		*offset = selected->pos - lines;
	}
	else if (selected->pos <= *offset)
	{
		clear_popup(lines);
		*offset = selected->pos - 1;
	}
	while (current->pos != (*offset + 1))
		current = current->next;
	while (current != current->next && current->pos <= lines + *offset)
	{
		if (current == selected)
			printf("\e[7m");
		switch (current->type) {
			case (DT_DIR):
				printf("\e[34m");
				break;
			case (DT_LNK):
				printf("\e[36m");
				break;
			default:
				printf("\e[39m");
				break;
		}
		printf("\e[%d;3H\e[1m%s%*s\e[22m ",
		 current->pos + start_line - *offset, current->old_name, longest_name - my_strlen(current->old_name), "");
		if (selected != current)
			printf("\e[m");
		printf(" %.*s/", env.TERM_COLS - (longest_name + 6), current->old_location);
		if (longest_name + 7 + my_strlen(current->old_location) < env.TERM_COLS)
			printf("%*s", env.TERM_COLS - (longest_name + 7 + my_strlen(current->old_location)), "");
		printf("\e[m");
		current = current->next;
	}
}

size_t	print_trash_size(char *buf, int lines, char *home)
{
	size_t		trash_size;

	sprintf(buf, "%s/.local/share/fse/trash", home);
	printf("\e[%d;11H", env.TERM_ROWS - (lines + 2));
	trash_size = recursive_dir_size_wrapper(get_vd_node(VISITED_DIRS, buf));
	format_filesize(trash_size);
	printf(" ]");
	return (trash_size);
}

void	navigate_trash(trash_node *head)
{
	trash_node	*selected = head->next;
	trash_node	*temp;
	char		*home = getenv("HOME");
	int			lines = count_trash_nodes(trash_list);
	char		c;
	char		buf[500];
	// char		*bufp;
	int			offset = 0;
	size_t		trash_size;

	if (selected == selected->next)
		selected = NULL;
	if (lines < 5)
		lines = 5;
	if (lines > env.TERM_ROWS / 2 - 2)
		lines = env.TERM_ROWS / 2 - 2;
	spawn_popup("Trash", lines);
	clear_gutter();
	display_trash(head, selected, lines, &offset);
	trash_size = print_trash_size(buf, lines, home);
	while ((c = getchar()) != binds.QUIT)
	{
		if (c == binds.OPEN_TRASH)
			return ;
		clear_gutter();
		if (selected == NULL)
		{
			print_trash_no(selected);
			continue ;
		}
		else if (c == binds.SELECT_NEXT)
		{
			if (selected->next != selected->next->next)
				selected = selected->next;
		}
		else if (c == binds.SELECT_PREV)
		{
			if (selected->prev != selected->prev->prev)
				selected = selected->prev;
		}
		else if (c == binds.DELETE)
		{
			temp = selected;
			offset = 0;
			clear_gutter();
			printf("\e[%d;3H[ \e[33mPermanently delete file? [y/N] :\e[m %s ]", env.TERM_ROWS, temp->old_name);
			if ((c = getchar()) == 'y')
			{
				if (selected->next != selected->next->next)
					selected = selected->next;
				else if (selected->prev != selected->prev->prev)
					selected = selected->prev;
				else
					selected = NULL;
				clear_gutter();
				sprintf(buf, "rm -rf %s/.local/share/fse/trash/%s", home, temp->name);
				printf("\e[%d;3H[ \e[33mPermanently deleted file:\e[m %s ]", env.TERM_ROWS, temp->old_name);
				system(buf);
				delete_selected_trash_node(head, temp);
				number_trash_nodes(head);
				write_trash_file();
				trash_size = print_trash_size(buf, lines, home);
				clear_popup(lines);
			}
			else
				clear_gutter();
		}
		else if (c == 'R')
		{
			temp = selected;
			offset = 0;
			clear_gutter();
			printf("\e[%d;3H[ \e[33mRestore file? [y/N] :\e[m %s -> %s/ ]", env.TERM_ROWS, temp->old_name, temp->old_location);
			if ((c = getchar()) == 'y')
			{
				if (selected->next != selected->next->next)
					selected = selected->next;
				else if (selected->prev != selected->prev->prev)
					selected = selected->prev;
				else
					selected = NULL;
				clear_gutter();
				sprintf(buf, "mv \"%s/.local/share/fse/trash/%s\" \"%s/%s\"", home, temp->name, temp->old_location, temp->old_name);
				printf("\e[%d;3H[ \e[33mRestored file: \e[m \"%s\" ]", env.TERM_ROWS, temp->old_name);
				system(buf);
				delete_selected_trash_node(head, temp);
				number_trash_nodes(head);
				write_trash_file();
				trash_size = print_trash_size(buf, lines, home);
				clear_popup(lines);
			}
			else
				clear_gutter();
		}
		else if (c == binds.CLEAR_BUF)
		{
			clear_gutter();
			printf("\e[%d;3H[ \e[33mClear trash folder? [y/N]\e[m ]", env.TERM_ROWS);
			if ((c = getchar()) == 'y')
			{
				selected = trash_list->next;
				while (selected != selected->next)
				{
					sprintf(buf, "rm -rf %s/.local/share/fse/trash/%s", home, selected->name);
					system(buf);
					delete_selected_trash_node(head, selected);
					selected = trash_list->next;
				}
				selected = NULL;
				clear_gutter();
				printf("\e[%d;3H[ \e[33;1mTrash cleared! \e[m Freed ", env.TERM_ROWS);
				format_filesize(trash_size);
				printf(" ]");
				write_trash_file();
				print_trash_size(buf, lines, home);
				clear_popup(lines);
			}
			else
				clear_gutter();
		}
		else
		{
			print_trash_no(selected);
			continue ;
		}
		display_trash(head, selected, lines, &offset);
	}
}