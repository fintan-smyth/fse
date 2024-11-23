#ifndef BOOKMARKS_H_
#define BOOKMARKS_H_

#include "structs.h"

typedef struct bookmark_node
{
	struct bookmark_node	*prev;
	struct bookmark_node	*next;
	char					*name;
	char					*path;
	int						pos;
} bookmark_node;

extern bookmark_node	*bookmarks;

bookmark_node	*init_bookmarks(void);
bookmark_node 	*insert_bookmark(char *name, char *path, bookmark_node *head);
int				check_bookmark_exists(char *path, bookmark_node *head);
void			delete_next_bookmark(bookmark_node *t);
void			free_bookmarks(bookmark_node *head);
void			write_bookmarks(void);
void			load_bookmarks_file(void);
int				count_bookmarks(bookmark_node *head);
void			display_bookmarks(bookmark_node *head, bookmark_node *selected, int lines, int *offset);
int				navigate_bookmarks(vd_node *dir_node, bookmark_node *head);


#endif // !BOOKMARKS_H_
