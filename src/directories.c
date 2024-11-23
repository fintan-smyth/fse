#include "headers/structs.h"
#include "headers/utils.h"
#include "headers/env.h"

vd_node	*VISITED_DIRS;

struct vd_node	*init_visited(void)
// Initialises a linked list of nodes containing information
// about directories that have been visited.
// Returns:
//  - Pointer to head of the linked list
{
	struct vd_node	*head;
	struct vd_node	*tail;

	head = (struct vd_node *)malloc(sizeof(*head));
	tail = (struct vd_node *)malloc(sizeof(*tail));
	head->next = tail;
	head->directory = NULL;
	head->dir_name = NULL;
	head->selected_name = NULL;
	head->children = NULL;
	tail->next = tail;
	tail->directory = NULL;
	tail->dir_name = NULL;
	tail->selected_name = NULL;
	tail->children = NULL;
	return (head);
}

vd_node	*vd_insert(vd_node *visited, char *dir_path)
// Inserts a node into the linked list of visited directories
// Args:
//  - visited:	pointer to the head of the visited directories list
//  - dir_name:	full path of directory
// Returns:
//  - Pointer to newly inserted node
{
	vd_node 			*new = malloc(sizeof(*new));

	new->next = visited->next;
	visited->next = new;
	new->directory = NULL;
	memset(new->search_term, 0, 255);
	new->dir_name = strdup(dir_path);
	new->selected_name = malloc(1 * sizeof(char));
	memset(new->selected_name, 0, 1 * sizeof(char));
	new->offset = 0;
	new->no_entries = 0;
	new->children = init_gen_list();
	return (new);
}

gen_node	*insert_child_vd(gen_node *children, vd_node *child)
{
	gen_node *new;

	new = malloc(sizeof(*new));
	new->data = child;
	new->next = children->next;
	children->next = new;
	return (new);
}


void	cleanup_vd_children(vd_node *dir_node)
{
	gen_node *temp;

	if (dir_node->children == NULL)
		return ;
	temp = dir_node->children;
	while (temp->next != temp->next->next)
	{
		cleanup_directory(temp->next->data);
		delete_next_gen(temp);
	}
}

void	delete_children_list(vd_node *dir_node)
{
	if (dir_node->children == NULL)
		return ;
	cleanup_vd_children(dir_node);
	free(dir_node->children->next);
	free(dir_node->children);
	dir_node->children = NULL;
}

int	check_visited(vd_node *visited, char *dir_path)
// Checks if a directory has already been visited
// Args:
//  - visited:	pointer to the head of the visited directories list
//  - dir_name:	full path of directory
// Returns:
//  - 1 if directory has been visited
//  - 0 if directory has not been visited
{
	vd_node	*current;

	current = visited->next;
	while (current != current->next)
	{
		if ((strncmp(current->dir_name, dir_path, strlen(current->dir_name)) == 0) && (strncmp(current->dir_name, dir_path, strlen(dir_path)) == 0))
			return (1);
		current = current->next;
	}
	return (0);
}

void	cleanup_directory(vd_node *dir_node)
// Frees resources associated with an open directory, allowing
// its contents to be refreshed.
// Args:
//  - dir_node:	pointer to node in visited directories list
{
	if (dir_node->directory == NULL)
		return ;
	free_entries(dir_node->directory->children);
	closedir(dir_node->directory->dir);
	free(dir_node->directory);
	memset(dir_node->search_term, 0, 255);
	dir_node->directory = NULL;
	cleanup_vd_children(dir_node);
}

struct directory	*get_directory(vd_node *dir_node)
// Creates a list of the entries in a directory, sorts them, and 
// puts the list in a 'directory' struct.
// Args:
//  - dir_node:	pointer to directory node of the directory to be retrieved
// Returns:
//  - 'directory' struct containing doubly linked list of entries
//  - NULL if directory has already been retrieved
{
	struct dirent		*child;
	struct directory	*directory;
	DIR					*dir;
	entry_node			*head;
	entry_node			*current;
	entry_node			**entry_array;
	int					left = 0;

	if (dir_node->directory != NULL)
	{
		return (NULL);
	}
	dir_node->no_entries = 0;
	directory = malloc(sizeof(*directory));
	head = init_list();
	directory->children = head;
	current = head;
 	dir = opendir(dir_node->dir_name);
	directory->dir = dir;
	if (dir == NULL)
	{
		free(directory);
		free_entries(head);
		return (NULL);
	}
	while ((child = readdir(dir)) != NULL)
	{
		current = insertafter(dir_node->dir_name, child, current);
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
			if ((env.FLAGS & F_HIDDEN) == 0)
			{
				delete_next_entry(current);
				continue;
			}
			current = current->next;
		}
		else
			current = current->next;
	}
	dir_node->no_entries = number_list(head);
	entry_array = malloc(dir_node->no_entries * sizeof(*entry_array));
	populate_entry_array(entry_array, head->next);
	if (env.FLAGS & F_HIDDEN)
	{
		int i = 0;
		while (i < dir_node->no_entries)
		{
			if (strcmp(entry_array[i]->data->d_name, ".") == 0)
				swap_entries(entry_array, i, 0);
			else if (strcmp(entry_array[i]->data->d_name, "..") == 0)
				swap_entries(entry_array, i, 1);
			i++;
		}
		left = 2;
	}
	q_sort(entry_array, left, dir_node->no_entries - 1);
	dir_node->no_entries = number_list(head);
	free(entry_array);
	dir_node->directory = directory;
	return (directory);
}

void	free_visited(vd_node *head)
// Frees the list of visited directories.
// Args:
//  - head: pointer to head of visited directories list
{
	vd_node	*current;
	vd_node	*temp;

	current = head->next;
	while (current != current->next)
	{
		free(current->dir_name);
		free(current->selected_name);
		delete_children_list(current);
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
// Retrieves the vd_node associated with a given directory absolute path.
// If no node exists, creates a new one.
// Args:
//  - visited:	pointer to head of visited directories list
//  - path:		full path of directory
// Returns:
//  - Pointer to the node associated with the given directory path
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
// Retrieves the vd_node of a given directory's parent.
// Args:
//  - dir_node:	node of the directory who's parent should be retrieved
// Returns:
//   - Pointer to the node of the parent directory
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

