#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/structs.h"

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
	head->data = NULL;
	head->pos = -5;
	tail->next = tail;
	tail->prev = head;
	tail->data = NULL;
	head->prev = head;
	return (head);
}

void    delete_next_entry(entry_node *t)
// Deletes a node from the given entry list.
// Args:
//  - t:	pointer to the node before the node to be deleted
{
	entry_node *temp;
	temp = t->next;
	t->next = t->next->next;
	t->next->prev = t;
	free(temp);
}

entry_node *insertafter(struct dirent *data, entry_node *t)
// Inserts a node into a list of entries.
// Args:
//  - data:	pointer to 'dirent' struct containing information about an entry
//  - t:	pointer to the node after which the new node will be inserted
// Returns:
//  - Pointer to the newly inserted node
{
	entry_node *new;

	new = (entry_node *)malloc(sizeof(*new));
	new->data = data;
	new->next = t->next;
	new->prev = t;
	new->lines = 0;
	t->next = new;
	new->next->prev = new;
	return (new);
}

void    free_entries(entry_node *head)
// Frees a list of entries.
// Args:
//  - head:	pointer to the head of a list of entries
{
    entry_node	*current;
    entry_node	*temp;

    current = head;
    while (current->next != current->next->next)
    {
        temp = current->next;
        free(current);
        current = temp;
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

	swap_nodes(entry_array[i], entry_array[j]);
	temp = entry_array[i];
	entry_array[i] = entry_array[j];
	entry_array[j] = temp;
}

int	comp_entries(entry_node *a, entry_node *b)
// Compares entries by type and then alphabetical order, for use in q_sort.
// Args:
//  - a:	pointer to entry to compare
//  - b:	pointer to entry to compare
// Returns:
//  - 1 if position of nodes should be swapped
//  - 0 if position of nodes should not be swapped
{
	int	type_comp = b->data->d_type - a->data->d_type;

	if (type_comp > 0)
		return (1);
	else if (type_comp == 0 && (strcasecmp(a->data->d_name, b->data->d_name) < 0))
		return (1);
	return (0);
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
	selected = children->next;
	if (children->next == children->next->next)
		return (NULL);
	if (*(dir_node->selected_name) != 0)
	{
		while (strncmp(dir_node->selected_name, selected->data->d_name, strlen(dir_node->selected_name)) || strncmp(dir_node->selected_name, selected->data->d_name, strlen(selected->data->d_name)))
		{
			selected = selected->next;
			if (selected == selected->next)
			{
				selected = children->next;
				break;
			}
		}
	}
	free(dir_node->selected_name);
	dir_node->selected_name = strdup(selected->data->d_name);
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
	entry_node	*children;
	entry_node	*current;

	children = dir_node->directory->children;
	current = children->next;
	if (children->next == children->next->next)
		return (NULL);
	if (*(dir_node->search_term) != 0)
	{
		while (strcasestr(current->data->d_name, dir_node->search_term) == NULL)
		{
			current = current->next;
			if (current == current->next)
			{
				return (NULL);
			}
		}
		free(dir_node->selected_name);
		dir_node->selected_name = strdup(current->data->d_name);
		return (current);
	}
	return (NULL);
}
