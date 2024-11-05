#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <dirent.h>

typedef struct entry_node
{
	struct entry_node	*prev;
	struct entry_node	*next;
	struct dirent		*data;
	int					pos;
	int					lines;
} entry_node;

typedef struct path_node
{
	struct path_node *next;
	char			 *path;
} path_node;

struct directory
{
	entry_node *children;
	DIR		   *dir;
};

typedef struct vd_node
{
	struct vd_node	 *next;
	struct directory *directory;
	int				  offset;
	char			 *dir_name;
	char			 *selected_name;
	char			  search_term[255];
} vd_node;

extern vd_node *VISITED_DIRS;

entry_node *init_list(void);
entry_node *insertafter(struct dirent *data, entry_node *t);
entry_node *get_selected(vd_node *dir_node);
entry_node *get_search_match(vd_node *dir_node);
void		delete_next_entry(entry_node *t);
void		free_entries(entry_node *head);
void		print_ll(entry_node *head);
void		swap_adj_nodes(entry_node *a, entry_node *b);
void		swap_distant_nodes(entry_node *a, entry_node *b);
void		swap_nodes(entry_node *a, entry_node *b);
void		swap_entries(entry_node **entry_array, int i, int j);
void		populate_entry_array(entry_node **entry_array, entry_node *first);
void		q_sort(entry_node **entry_array, int left, int right);
int			number_list(entry_node *head);
int			comp_entries(entry_node *a, entry_node *b);

struct directory *get_directory(char *dir_name);
vd_node			 *init_visited(void);
vd_node			 *vd_insert(vd_node *visited, char *dir_name);
vd_node			 *get_vd_node(vd_node *visited, char *path);
vd_node			 *get_parent(vd_node *dir_node);
void			  cleanup_directory(vd_node *dir_node);
void			  free_visited(vd_node *head);
int				  check_visited(vd_node *visited, char *dir_name);

extern path_node *copied;
extern path_node *cut;

path_node	*init_path_list(void);
path_node	*insert_path_node(char *path, path_node *t);
void		delete_next_path(path_node *t);
void		free_path_list(path_node *head);
void		clear_path_list(path_node *head);
int			check_path(path_node *copied, char *path);
int			delete_path(char *path, path_node *head);
char		*construct_path(char *buf, char *directory, char *file_name);
char		*get_file_name(char *path);
int			check_path_exists(vd_node *dir_node, char *path);
int			check_file_exists(vd_node *dir_node, char *path);
void		paste(vd_node *dir_node);
void		print_copied(path_node *copied);
void		print_file_attributes(entry_node *entry);

#endif // !STRUCTS_H_
