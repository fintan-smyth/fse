#include "headers/structs.h"
#include "headers/env.h"
#include "headers/utils.h"
#include <string.h>

entry_node *init_list(void)
// Initialises a doubly linked list of nodes containing information
// about the entries in a directory.
// Returns:
//  - Pointer to head of the entry list
{
	entry_node *head;
	entry_node *tail;

	head = (entry_node *)malloc(sizeof(*head));
	tail = (entry_node *)malloc(sizeof(*tail));
	head->next = tail;
	head->d_name = NULL;
	head->attr = NULL;
	head->pos = -5;
	tail->next = tail;
	tail->prev = head;
	tail->d_name = NULL;
	tail->attr = NULL;
	head->prev = head;
	return (head);
}

void	delete_next_entry(entry_node *t)
// Deletes the next node from the given entry list.
// Args:
//  - t:	pointer to the node before the node to be deleted
{
	entry_node *temp;
	temp = t->next;
	t->next = t->next->next;
	t->next->prev = t;
	if (temp->attr != NULL)
		free(temp->attr);
	if (temp->d_name != NULL)
		free(temp->d_name);
	free(temp);
}

void	copy_dirent(entry_node *t, struct dirent *data)
{
	// struct dirent *new = malloc(sizeof(*data));

	t->d_type = data->d_type;
	t->d_name = strdup(data->d_name);
	// return (new);
}

entry_node *insertafter(char *dir_path, struct dirent *data, entry_node *t)
// Inserts a node into a list of entries.
// Args:
//  - dir_path:	absolute path of directory containing entry
//  - data:		pointer to 'dirent' struct containing information about an entry
//  - t:		pointer to the node after which the new node will be inserted
// Returns:
//  - Pointer to the newly inserted node
{
	entry_node		*new;
	struct stat 	*attr;
	char			abs_path[500];

	if (data == NULL)
		return (NULL);
	else if (!str_printable(data->d_name))
		return (NULL);
	else if (str_is_whitespace(data->d_name))
		return (NULL);
	new = (entry_node *)malloc(sizeof(*new));
	attr = malloc(sizeof(*attr));
	copy_dirent(new, data);
	construct_path(abs_path, dir_path, data->d_name);
	if (stat(abs_path, attr) == -1)
	{
		new->attr = NULL;
		free(attr);
	}
	else
	{
		new->attr = attr;
	}
	new->next = t->next;
	new->prev = t;
	new->lines = 0;
	t->next = new;
	new->next->prev = new;
	return (new);
}

void	free_entries(entry_node *head)
// Frees a list of entries.
// Args:
//  - head:	pointer to the head of a list of entries
{
	entry_node	*current;
	entry_node	*temp;

	current = head;
	while (current->next != current->next->next)
	{
		delete_next_entry(current);
	}
	temp = current->next;
	free(current);
	free(temp);
}

void	swap_adj_nodes(entry_node *a, entry_node *b)
// Swaps the position of two adjacent entry nodes.
// Args:
//  - a:	pointer to the node earlier in the list
//  - b:	pointer to the node later in the list
{
	entry_node	*temp = malloc(sizeof(*temp));

	temp->next = b->next;
	temp->prev = a->prev;

	a->prev->next = b;
	a->next->next->prev = b;
	a->prev = b;

	b->next->prev = a;
	b->next = a;
	b->prev = temp->prev;
	a->next = temp->next;
	free(temp);
}

void	swap_distant_nodes(entry_node *a, entry_node *b)
// Swaps the position of two non-adjacent entry nodes.
// Args:
//  - a:	pointer to the node earlier in the list
//  - b:	pointer to the node later in the list
{
	entry_node	*temp = malloc(sizeof(*temp));
	
	temp->next = b->next;
	temp->prev = b->prev;

	a->prev->next = b;
	a->next->prev = b;
	b->next = a->next;
	b->prev = a->prev;

	a->next = temp->next;
	a->prev = temp->prev;
	a->next->prev = a;
	a->prev->next = a;
	free(temp);
}

void	swap_nodes(entry_node *a, entry_node *b)
// Swaps the position of any two nodes in an entry list.
// Args:
//  - a:	pointer to a node in the entry list
//  - b:	pointer to a node in the entry list
{
	entry_node	*first;
	entry_node	*second;
	entry_node	*current;

	current = a;
	while (current != current->next)
	{
		if (current == b)
		{
			first = a;
			second = b;
			break;
		}
		current = current->next;
	}
	if (current == current->next)
	{
		first = b;
		second = a;
	}
	if (first->next == second)
		swap_adj_nodes(first, second);
	else
		swap_distant_nodes(first, second);
}

void	swap_entries(entry_node **entry_array, int i, int j)
// Swaps the position of two entries in the sort array.
// Args:
//  - entry_array:	array of pointers to entry nodes, to be used for sorting
//  - i:			index of entry in sort array
//  - j:			index of entry in sort array
{
	entry_node	*temp;

	if (i == j)
		return ;
	// swap_nodes(entry_array[i], entry_array[j]);
	temp = entry_array[i];
	entry_array[i] = entry_array[j];
	entry_array[j] = temp;
}

int	comp_entries(entry_node *right, entry_node *left)
// Compares entries by type and then alphabetical order, for use in q_sort.
// Args:
//  - a:	pointer to entry to compare
//  - b:	pointer to entry to compare
// Returns:
//  - 1 if position of nodes should be swapped
//  - 0 if position of nodes should not be swapped
{
	int	type_comp = left->d_type - right->d_type;
	if (type_comp > 0)
		return (1);
	if (type_comp < 0)
		return (0);
	switch (env.SORT) {
		case (SORT_SIZE):
			if (right->attr == NULL)
				return (Apply_reverse(0));
			if (left->attr == NULL)
				return (Apply_reverse(1));
			if (right->attr->st_size > left->attr->st_size)
				return (Apply_reverse(1));
			break;
		case (SORT_MTIME):
			if (right->attr == NULL)
				return (Apply_reverse(0));
			if (left->attr == NULL)
				return (Apply_reverse(1));
			if (right->attr->st_mtime > left->attr->st_mtime)
				return (Apply_reverse(1));
			break;
		case (SORT_ATIME):
			if (right->attr == NULL)
				return (Apply_reverse(0));
			if (left->attr == NULL)
				return (Apply_reverse(1));
			if (right->attr->st_atime > left->attr->st_atime)
				return (Apply_reverse(1));
			break;
		default:
			if (strcasecmp(right->d_name, left->d_name) < 0)
				return (Apply_reverse(1));
			break;
	}
	return (Apply_reverse(0));
}

void q_sort(entry_node **entry_array, int left, int right)
// Stably quicksorts a list of entries by type and then alphabetical order.
// Args:
//  - entry_array:	array of pointers to entry nodes, to be used for sorting
//  - left:			index of leftmost entry to sort
//  - right:		index of rightmost entry to sort
{
	int i, last;

	if (left >= right)
		return;
	swap_entries(entry_array, left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
	{
		if (comp_entries(entry_array[i], entry_array[left]))
			swap_entries(entry_array, ++last, i);
	}
	swap_entries(entry_array, left, last);
	q_sort(entry_array, left, last - 1);
	q_sort(entry_array, last + 1, right);
}

int	number_list(entry_node *head)
// Numbers the entries in a list based on their position in the list.
// Args:
//  - head:	pointer to head of the entry list to number
// Returns:
//  - Number of entries in the list
{
	entry_node	*current;
	int			i;

	current = head->next;
	i = 0;
	while (current != current->next)
	{
		current->pos = ++i;
		current = current->next;
	}
	return (i);
}

void number_entry_array(entry_node **entry_array, int no_entries)
{
	int	i = 0;

	while (i < no_entries)
	{
		entry_array[i]->pos = i + 1;
		i++;
	}
}

void	populate_entry_array(entry_node **entry_array, entry_node *first)
// Populates the array used for sorting an entry list.
// Args:
//  - entry_array:	array of pointers to entry nodes, to be used for sorting
//  - first:		pointer to the first entry in an entry list (i.e one after the head)
{
	entry_node	*current;
	int			i = 0;

	current = first;
	while (current != current->next)
	{
		entry_array[i] = current;
		i++;
		current = current->next;
	}
}

entry_node	*get_selected(vd_node *dir_node)
// Retrieves the node of the selected entry in a directory.
// Args:
//  - dir_node:	pointer to the vd_node who's selected entry should be retrieved
// Returns:
//  - Pointer to the entry_node of the selected entry
{
	entry_node	*children;
	entry_node	*selected;

	children = dir_node->directory->children;
	if (dir_node->no_entries == 0)
		return (NULL);
	// if (children->next == children->next->next)
	// return (NULL);
	if (*(dir_node->selected_name) != 0)
	{
		selected = dir_node->directory->children->next;
		while (strncmp(dir_node->selected_name, selected->d_name, strlen(dir_node->selected_name)) || strncmp(dir_node->selected_name, selected->d_name, strlen(selected->d_name)))
		{
			selected = selected->next;
			if (selected == selected->next)
			{
				selected = children->next;
				break;
			}
		}
	}
	else
		selected = dir_node->entry_array[0];
	free(dir_node->selected_name);
	dir_node->selected_name = strdup(selected->d_name);
	return (selected);
}

entry_node	*get_search_match(vd_node *dir_node)
// Finds the first entry in a directory matching the search term
// Args:
//  - dir_node: pointer to search directory
// Returns:
//  - pointer to first entry node matching search term
//  - NULL if no entry matches search term
{
	entry_node	*current;
	int			i = 0;

	if (dir_node->no_entries == 0)
		return (NULL);
	current = dir_node->entry_array[0];
	if (*(dir_node->search_term) != 0)
	{
		while (strcasestr(current->d_name, dir_node->search_term) == NULL)
		{
			if (++i == dir_node->no_entries)
				return (NULL);
			current = dir_node->entry_array[i];
		}
		if (i == dir_node->no_entries)
			return (NULL);
		free(dir_node->selected_name);
		dir_node->selected_name = strdup(current->d_name);
		return (current);
	}
	return (NULL);
}
