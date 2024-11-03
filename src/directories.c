#include "headers/structs.h"
#include "headers/utils.h"

extern int FLAG_HIDDEN;
vd_node	*VISITED_DIRS;

struct vd_node	*init_visited(void)
{
	struct vd_node	*head;
	struct vd_node	*tail;

	head = (struct vd_node *)malloc(sizeof(*head));
	tail = (struct vd_node *)malloc(sizeof(*tail));
	head->next = tail;
	head->directory = NULL;
	head->dir_name = NULL;
	head->selected_name = NULL;
	tail->next = tail;
	tail->directory = NULL;
	tail->dir_name = NULL;
	tail->selected_name = NULL;
	return (head);
}

vd_node	*vd_insert(vd_node *visited, char *dir_name)
{
	vd_node 			*new = malloc(sizeof(*new));

	new->next = visited->next;
	visited->next = new;
	new->directory = NULL;
	memset(new->search_term, 0, 255);
	new->dir_name = strdup(dir_name);
	new->selected_name = malloc(1 * sizeof(char));
	memset(new->selected_name, 0, 1 * sizeof(char));
	new->offset = 0;
	return (new);
}

int	check_visited(vd_node *visited, char *dir_name)
{
	vd_node	*current;

	current = visited->next;
	while (current != current->next)
	{
		if ((strncmp(current->dir_name, dir_name, strlen(current->dir_name)) == 0) && (strncmp(current->dir_name, dir_name, strlen(dir_name)) == 0))
			return (1);
		current = current->next;
	}
	return (0);
}

void	cleanup_directory(vd_node *dir_node)
{
	free_entries(dir_node->directory->children);
	closedir(dir_node->directory->dir);
	free(dir_node->directory);
	memset(dir_node->search_term, 0, 255);
	dir_node->directory = NULL;
}

struct directory	*get_directory(char *dir_name)
{
	struct dirent		*child;
	struct directory	*directory;
	DIR					*dir;
	entry_node			*head;
	entry_node			*current;
	entry_node			**entry_list;
	int					no_entries;

	directory = malloc(sizeof(*directory));
	head = init_list();
	directory->children = head;
	current = head;
 	dir = opendir(dir_name);
	directory->dir = dir;
	if (dir == NULL)
	{
		free(directory);
		free_entries(head);
		return (NULL);
	}
	while ((child = readdir(dir)) != NULL)
	{
		current = insertafter(child, current);
	}
	current = head;
	while (current->next != current->next->next)
	{
		if (!str_printable(current->next->data->d_name))
			delete_next_entry(current);
		else if (current->next->data->d_type < 4 || current->next->data->d_type > 10)
			delete_next_entry(current);
		else if (strncmp(current->next->data->d_name, ".", 1) == 0)
		{
			if (FLAG_HIDDEN == 0)
			{
				delete_next_entry(current);
				continue;
			}
			current = current->next;
		}
		else
			current = current->next;
	}
	no_entries = number_list(head);
	entry_list = malloc(no_entries * sizeof(*entry_list));
	populate_entry_list(entry_list, head->next);
	q_sort(entry_list, 0, no_entries - 1);
	number_list(head);
	free(entry_list);
	return (directory);
}

void	free_visited(vd_node *head)
{
	vd_node	*current;
	vd_node	*temp;

	current = head->next;
	while (current != current->next)
	{
		free(current->dir_name);
		free(current->selected_name);
		current = current->next;
	}
	current = head;
	while (current != current->next)
	{
		temp = current;
		current = current->next;
		free(temp);
	}
	free(current);
}

vd_node	*get_vd_node(vd_node *visited, char *path)
{
	vd_node	*current;

	if (!check_visited(visited, path))
	{
		current = vd_insert(visited, path);
	}
	else
	{
		current = VISITED_DIRS->next;
		while ((strncmp(path, current->dir_name, strlen(path)) || strncmp(path, current->dir_name, strlen(current->dir_name))))
			current = current->next;
	}
	return (current);
}

vd_node	*get_parent(vd_node *dir_node)
{
	vd_node	*parent;
	int		i;
	char	parent_path[255] = {0};
	char	parent_selected[255] = {0};

	i = strlen(dir_node->dir_name);
	while (dir_node->dir_name[i] != '/')
		i--;
	strncpy(parent_path, dir_node->dir_name, i ? i : 1);
	strncpy(parent_selected, &(dir_node->dir_name[i + 1]), strlen(dir_node->dir_name) - i - 1);
	parent = get_vd_node(VISITED_DIRS, parent_path);
	if (*(parent->selected_name) == 0)
	{
		free(parent->selected_name);
		parent->selected_name = strndup(parent_selected, strlen(parent_selected));
	}
	return (parent);
}

