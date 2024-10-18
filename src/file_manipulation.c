#include "headers/structs.h"
#include "headers/format.h"
#include "headers/utils.h"

path_node	*copied;
path_node	*cut;

path_node	*init_path_list(void)
{
	path_node	*head;
	path_node	*tail;

	head = (path_node *)malloc(sizeof(*head));
	tail = (path_node *)malloc(sizeof(*tail));
	head->next = tail;
	tail->next = tail;
	head->path = NULL;
	tail->path = NULL;
	return (head);
}

void	delete_next_path(path_node *t)
{
	path_node	*temp;
	temp = t->next;
	t->next = t->next->next;
	free(temp->path);
    free(temp);
}

path_node	*insert_path_node(char *path, path_node *t)
{
	path_node	*new;

	new = malloc(sizeof(*new));
	new->path = strdup(path);
	new->next = t->next;
	t->next = new;
	return (new);
}

void	free_path_list(path_node *head)
{
	path_node	*current;
	path_node	*temp;

	current = head->next;
	free(head);
	while (current->next != current->next->next)
	{
		temp = current->next;
		free(current->path);
		free(current);
		current = temp;
	}
	temp = current->next;
	if (current->path != NULL)
	{
		free(current->path);
		free(current);
	}
	free(temp);
}

int	check_path(path_node *copied, char *path)
{
	path_node	*current;

	if (copied->next == copied->next->next)
		return (0);
	current = copied->next;
	while (current != current->next)
	{
		if (strcmp(path, current->path) == 0)
			return(1);
		current = current->next;
	}
	return (0);
}

int	delete_path(char *path, path_node *head)
{
	path_node	*current;

	current = head;
	while (current->next != current->next->next)
	{
		if (strcmp(path, current->next->path) == 0)
		{
			delete_next_path(current);
			return (0);
		}
		current = current->next;
	}
	return (1);
}

char	*construct_path(char *buf, char *directory, char *file_name)
{
	if (my_strlen(directory) == 1 && *directory == '/')
	{
		sprintf(buf, "/%s", file_name);
	}
	else {
		sprintf(buf, "%s/%s", directory, file_name);
	}
	return (buf);
}

char	*get_file_name(char *path)
{
	int		i;
	char	*file_name;

	i = my_strlen(path) - 1;
	while (path[i] != '/')
	{
		i--;
	}
	file_name = strdup(&path[i + 1]);
	return (file_name);
}

int	check_file_exists(vd_node *dir_node, char *path)
{
	char	*file_name;
	entry_node *current = dir_node->directory->children->next;


	file_name = get_file_name(path);
	while (current != current->next)
	{
		if (strcmp(file_name, current->data->d_name) == 0)
		{
			free(file_name);
			return (1);
		}
		current = current->next;
	}
	free(file_name);
	return (0);
}

void	paste(vd_node *dir_node)
{
	path_node	*current;
	char		command_buf[500];
	char		*file_name;
	char		c;

	current = copied;
	while (current->next != current->next->next)
	{
		if (check_file_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", TERM_ROWS);
		}
		sprintf(command_buf, "cp -r %s %s", current->next->path, dir_node->dir_name);
		system(command_buf);
		memset(command_buf, 0, 500);
		delete_next_path(current);
	}
	current = cut;
	while (current->next != current->next->next)
	{
		if (check_file_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", TERM_ROWS);
		}
		file_name = get_file_name(current->next->path);
		construct_path(command_buf, dir_node->dir_name, file_name);
		rename(current->next->path, command_buf);
		memset(command_buf, 0, 500);
		free(file_name);
		delete_next_path(current);
	}
}

void	print_copied(path_node *copied)
{
	path_node *current = copied->next;
	printf("<addr: %p>\t<next: %p>\t<path: %s>\n",copied, copied->next, copied->path);
	while (current != current->next)
	{
		printf("<addr: %p>\t<next: %p>\t<path: %s>\n",current, current->next, current->path);
		current = current->next;
	}
	printf("<addr: %p>\t<next: %p>\t<path: %s>\n",current, current->next, current->path);
}

