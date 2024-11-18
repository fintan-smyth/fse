#include "headers/bookmarks.h"
#include "headers/env.h"
#include "headers/format.h"
#include "headers/utils.h"

bookmark_node	*bookmarks;

bookmark_node *init_bookmarks(void)
// Initialises a doubly linked list of nodes containing information
// about the user's bookmarks.
// Returns:
//  - Pointer to head of the bookmark list
{
	bookmark_node *head;
	bookmark_node *tail;

	head = (bookmark_node *)malloc(sizeof(*head));
	tail = (bookmark_node *)malloc(sizeof(*tail));
	head->next = tail;
	head->prev = head;
	head->name = NULL;
	head->path = NULL;
	head->pos = 0;
	tail->pos = -5;
	tail->next = tail;
	tail->prev = head;
	tail->name = NULL;
	tail->path = NULL;
	return (head);
}

void	delete_next_bookmark(bookmark_node *t)
// Deletes the next node from the given bookmark list.
// Args:
//  - t:	pointer to the node before the node to be deleted
{
	bookmark_node *temp;
	temp = t->next;
	t->next = t->next->next;
	t->next->prev = t;
	free(temp->name);
	free(temp->path);
	free(temp);
}

void	delete_selected_bookmark(bookmark_node *head, bookmark_node *to_delete)
// Deletes the given node from a given bookmark list.
// Args:
//  - head:			pointer to the head of the bookmarks list
//  - to_delete:	pointer to the node to be deleted
{
	bookmark_node	*current;

	current = head;
	while (current->next != to_delete && current != current->next)
		current = current->next;
	if (current == current->next)
		return ;
	delete_next_bookmark(current);
	return ;
}

bookmark_node *insert_bookmark(char *name, char *path, bookmark_node *head)
// Inserts a node at the end of the bookmarks list.
// Args:
//  - name:		string containing the name of the bookmark
//  - path:		string containing the path of the bookmark
//  - t:		pointer to the head of the bookmarks list
// Returns:
//  - Pointer to the newly inserted node
{
	bookmark_node	*new;
	bookmark_node	*current;

	if (check_bookmark_exists(path, head))
		return (NULL);
	new = (bookmark_node *)malloc(sizeof(*new));
	current = head;
	while (current->next != current->next->next)
		current = current->next;
	new->next = current->next;
	new->prev = current;
	new->name = strdup(name);
	new->path = strdup(path);
	new->pos = current->pos + 1;
	current->next = new;
	new->next->prev = new;
	return (new);
}

int	check_bookmark_exists(char *path, bookmark_node *head)
// Checks if a bookmark with the given path exists.
// Args:
//  - path:	string containing the path to check
//  - head:	pointer to the head of the bookmarks list
// Returns:
//  - 1 if a bookmark with this path exists
//  - 0 if no bookmark with this path exists
{
	bookmark_node *current;

	current = head->next;
	while (current != current->next)
	{
		if (strcmp(path, current->path) == 0)
			return (1);
		current = current->next;
	}
	return (0);
}

void	free_bookmarks(bookmark_node *head)
// Frees a list of bookmarks.
// Args:
//  - head:	pointer to the head of a list of bookmarks.
{
	while (head->next != head->next->next)
		delete_next_bookmark(head);
	free(head->next);
	free(head);
}

void	write_bookmarks(void)
// Writes the current state of the bookmarks list to a file to allow persistence
// between sessions.
{
	bookmark_node	*current;
	FILE			*fptr;
	char			*home = getenv("HOME");
	char			path[140];

	strncpy(path, home, 100);
	strcat(path, "/.local/share/fse/bookmarks");
	fptr = fopen(path, "w");
	if (fptr == NULL)
	{
		printf("\e[2J\e[HError!\n");
		exit(0);
	}
	current = bookmarks->next;
	while (current != current->next)
	{
		fprintf(fptr, "%s\t%s\n", current->name, current->path);
		current = current->next;
	}
	fclose(fptr);
}

void	load_bookmarks_file(void)
// Loads in the bookmarks list from file.
{
	FILE			*fptr;
	char			*home = getenv("HOME");
	char			path[140];
	char			bm_path[200];
	char			bm_name[200];
	char			*line;
	size_t			size = 500;
	int				i;
	int				j;

	strncpy(path, home, 100);
	strcat(path, "/.local/share/fse/bookmarks");
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
			bm_name[j++] = line[i++];
		bm_name[j] = 0;
		j = 0;
		i++;
		while (line[i] != '\n')
			bm_path[j++] = line[i++];
		bm_path[j] = 0;
		insert_bookmark(bm_name, bm_path, bookmarks);
	}
	fclose(fptr);
	free(line);
}

int	count_bookmarks(bookmark_node *head)
// Counts the number of bookmarks in the bookmarks list.
// Args:
//  - head:	pointer to the head of the bookmarks list
// Returns:
//  - The number of bookmarks in the list
{
	bookmark_node	*current;

	current = head;
	while (current->next != current->next->next)
		current = current->next;
	return (current->pos);
}

void	number_bookmarks(bookmark_node *head)
// Numbers the position of each bookmark in the list.
// Args:
//  - head:	pointer to the head of the bookmarks list
{
	bookmark_node	*current;
	int				i;

	current = head->next;
	i = 1;
	while (current != current->next)
	{
		current->pos = i++;
		current = current->next;
	}
}

int	longest_bookmark_name(bookmark_node *head)
// Returns the length of the longest bookmark name.
// Args:
//  - head:	pointer to the head of the bookmarks list
// Returns:
//  - Length of the longest bookmark name
//  - 0 if there are no bookmarks
{
	bookmark_node	*current;
	int				len = 0;

	current = head->next;
	while (current != current->next)
	{
		if (my_strlen(current->name) > len)
			len = my_strlen(current->name);
		current = current->next;
	}
	return (len);
}

void	display_bookmarks(bookmark_node *head, bookmark_node *selected, int lines, int *offset)
// Displays the bookmarks list.
// Args:
//  - head:		pointer to the head of the bookmarks list
//  - selected:	pointer to the selected bookmark node
//  - lines:	the number of lines in the bookmarks popup
//  - offset:	pointer to the number of bookmarks to skip when displaying bookmarks
{
	bookmark_node	*current;
	int				start_line = env.TERM_ROWS - (lines + 2);
	int				longest_name = longest_bookmark_name(head);

	current = head->next;
	// clear_popup(lines);
	if (selected == NULL)
		return ;
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
		printf("\e[%d;3H\e[36;1m%s%*s\e[22m ",
		 current->pos + start_line - *offset, current->name, longest_name - my_strlen(current->name), "");
		if (selected != current)
			printf("\e[m");
		printf(" %.*s", env.TERM_COLS - (longest_name + 6), current->path);
		if (longest_name + 6 + my_strlen(current->path) < env.TERM_COLS)
			printf("%*s", env.TERM_COLS - (longest_name + 6 + my_strlen(current->path)), "");
		printf("\e[m");
		current = current->next;
	}
}

void	navigate_bookmarks(bookmark_node *head)
// Navigate and edit the user's bookmarks.
// Args:
//  - head:	pointer to the head of the bookmarks list
{
	bookmark_node	*selected = head->next;
	bookmark_node	*temp;
	int				lines = count_bookmarks(bookmarks);
	char			c;
	char			buf[500];
	char			*bufp;
	int				offset = 0;;

	if (selected == selected->next)
		selected = NULL;
	if (lines < 5)
		lines = 5;
	else if (lines > env.TERM_ROWS / 2)
		lines = env.TERM_ROWS / 2;
	spawn_popup("Bookmarks", lines);
	clear_gutter();
	display_bookmarks(head, selected, lines, &offset);
	while ((c = getchar()) != binds.QUIT)
	{
		if (c == 'b')
			return ;
		if (selected == NULL)
			continue ;
		else if (c == binds.SELECT_NEXT)
		{
			if (selected->next == selected->next->next)
				continue ;
			selected = selected->next;
		}
		else if (c == binds.SELECT_PREV)
		{
			if (selected->prev == selected->prev->prev)
				continue ;
			selected = selected->prev;
		}
		else if (c == binds.OPEN || c == '\n')
		{
			chdir(selected->path);
			return ;
		}
		else if (c == binds.DELETE)
		{
			temp = selected;
			if (selected->next != selected->next->next)
				selected = selected->next;
			else if (selected->prev != selected->prev->prev)
				selected = selected->prev;
			else
				selected = NULL;
			printf("\e[%d;3H[ \e[33mDeleted bookmark:\e[m \"%s\" ]", env.TERM_ROWS, temp->name);
			delete_selected_bookmark(head, temp);
			number_bookmarks(head);
			write_bookmarks();
			clear_popup(lines);
		}
		else if (c == binds.RENAME)
		{
			clear_gutter();
			bufp = buf;
			printf("\e[%d;3H[%*s]\e[4G \e[33mrename:\e[m ", env.TERM_ROWS, (env.TERM_COLS) - 6, "");
			printf("\e[?25h");
			while ((c = getchar()) != '\n')
			{
				*(bufp++) = c;
				printf("%c", c);
			}
			*bufp = 0;
			set_term_settings();
			if (my_strlen(buf) == 0 || c == 27 || str_printable(buf) == 0)
			{
				clear_gutter();
				printf("\e[%d;3H[ \e[33mFile not renamed\e[m ]", env.TERM_ROWS);
				continue ;
			}
			clear_gutter();
			printf("\e[%d;3H[ \e[33mRenamed bookmark:\e[m %s -> %s ]", env.TERM_ROWS, selected->name, buf);
			free(selected->name);
			selected->name = strdup(buf);
			write_bookmarks();
		}
		else
			return ;
		display_bookmarks(head, selected, lines, &offset);
	}
	return ;
}
