#ifndef TRASH_H_
#define TRASH_H_

#include "structs.h"

typedef struct trash_node
{
	struct trash_node	*prev;
	struct trash_node	*next;
	char				*name;
	char				*old_name;
	char				*old_location;
	int					type;
	int					pos;
} trash_node;

extern trash_node	*trash_list;

trash_node *init_trash_list(void);
void	delete_next_trash_node(trash_node *t);
trash_node *insert_trash_node(char *name, char *old_name, char *old_location, int type, trash_node *head);
trash_node *insert_selected_trash(vd_node *dir_node, entry_node *selected, trash_node *head);
void	free_trash(trash_node *head);
void	write_trash_file(void);
void	load_trash_file(void);
void	navigate_trash(trash_node *head);

#endif // !TRASH_H_
