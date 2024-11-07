#include "headers/structs.h"
#include "headers/env.h"
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
}

int	main(void)
{
	int				size = 500 * sizeof(char);
	char			cwd_name[500];
	vd_node			*current;
	
	env.FLAGS |= F_PREVIEW;
	set_default_binds();
	if (load_config() == 1)
	{
		printf("\e[2J\e[HError: invalid config\n");
		exit(1);
	}
	VISITED_DIRS = init_visited();
	copied = init_path_list();
	cut = init_path_list();
	store_term_settings();
	set_term_settings();
	atexit(exit_cleanup);
	do {
		getcwd(cwd_name, size);
		current = get_vd_node(VISITED_DIRS, cwd_name);
		current->directory = get_directory(cwd_name);
	} while (navigate(current) == 0);
	printf("\e[2J\e[H");
}
