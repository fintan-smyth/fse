#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/structs.h"


entry_node *init_list(void)
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
{
    entry_node *temp;
    temp = t->next;
	t->next = t->next->next;
	t->next->prev = t;
    free(temp);
}

entry_node *insertafter(struct dirent *data, entry_node *t)
{
	entry_node *new;

	new = (entry_node *)malloc(sizeof(*new));
	new->data = data;
	new->next = t->next;
	new->prev = t;
	t->next = new;
	new->next->prev = new;
	return (new);
}

void    free_entries(entry_node *head)
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

void    print_ll(entry_node *head)
{
	entry_node *current;

	// printf("<addr: %p>\t<next: %p>\t<prev: %p>\t<data: %4d \"%s\">\n", head, head->next, head->prev, head->pos, NULL);
	current = head->next;
	while (current != current->next)
	{
		printf("<addr: %p>\t<next: %p>\t<prev: %p>\t<data: %4d %4d \"%s\">\n", current, current->next, current->prev, current->pos, current->data->d_type, current->data->d_name);
		current = current->next;
	}
	// printf("<addr: %p>\t<next: %p>\t<prev: %p>\t<data: %4d \"%s\">\n", current, current->next, current->prev, current->pos, NULL);
}

void	swap_adj_nodes(entry_node *a, entry_node *b)
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

void	swap_entries(entry_node **entry_list, int i, int j)
{
	entry_node	*temp;

	swap_nodes(entry_list[i], entry_list[j]);
	temp = entry_list[i];
	entry_list[i] = entry_list[j];
	entry_list[j] = temp;
}

int	comp_entries(entry_node *a, entry_node *b)
{
	int	type_comp = b->data->d_type - a->data->d_type;

	if (type_comp > 0)
		return (1);
	else if (type_comp == 0 && (strcmp(a->data->d_name, b->data->d_name) < 0))
		return (1);
	return (0);
}

void q_sort(entry_node **entry_list, int left, int right)
{
	int i, last;

	if (left >= right)
		return;
	swap_entries(entry_list, left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
	{
		if (comp_entries(entry_list[i], entry_list[left]))
			swap_entries(entry_list, ++last, i);
	}
	swap_entries(entry_list, left, last);
	q_sort(entry_list, left, last - 1);
	q_sort(entry_list, last + 1, right);
}

int	number_list(entry_node *head)
{
	entry_node	*current;
	int		i;

	current = head->next;
	i = 0;
	while (current != current->next)
	{
		current->pos = ++i;
		current = current->next;

	}
	return (i);
}

void	populate_entry_list(entry_node **entry_list, entry_node *first)
{
	entry_node	*current;
	int		i = 0;

	current = first;
	while (current != current->next)
	{
		entry_list[i] = current;
		i++;
		current = current->next;
	}
}

entry_node	*get_selected(vd_node *dir_node)
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
