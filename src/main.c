#include "headers/structs.h"
#include "headers/env.h"
#include "headers/bookmarks.h"
#include "headers/trash.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	navigate(vd_node *dir_node);

void	exit_cleanup()
// Frees global lists and restores terminal settings on exit.
{
	reset_term_settings();
	free_visited(VISITED_DIRS);
	free_path_list(copied);
	free_path_list(cut);
	free_bookmarks(bookmarks);
	free_trash(trash_list);
}

int	main(void)
{
	int				size = 500 * sizeof(char);
	char			cwd_name[500];
	vd_node			*current;
	
	env.FLAGS = 0;
	env.SORT = 0;
	env.FLAGS |= F_PREVIEW;
	set_default_binds();
	if (load_config() == 1)
	{
		printf("Config error: invalid syntax\n");
		return (1);
	}
	if (check_double_binds() == 1)
	{
		printf("Config error: cannot have multiple binds to single key\n");
		return (1);
	}
	VISITED_DIRS = init_visited();
	copied = init_path_list();
	cut = init_path_list();
	bookmarks = init_bookmarks();
	trash_list = init_trash_list();
	load_bookmarks_file();
	load_trash_file();
	// check_trash_nodes_valid(trash_list);
	// check_unindexed_trash(trash_list);
	store_term_settings();
	set_term_settings();
	atexit(exit_cleanup);
	do {
		getcwd(cwd_name, size);
		current = get_vd_node(VISITED_DIRS, cwd_name);
		cleanup_directory(current);
		get_directory(current);
	} while (navigate(current) == 0);
	printf("\e[2J\e[H");
}
