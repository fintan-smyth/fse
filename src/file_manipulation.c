#include "headers/structs.h"
#include "headers/format.h"
#include "headers/utils.h"
#include <sys/stat.h>

path_node	*copied;
path_node	*cut;
path_node	*highlighted;

path_node	*init_path_list(void)
{
	path_node	*head;
	path_node	*tail;

	head = (path_node *)malloc(sizeof(*head));
	tail = (path_node *)malloc(sizeof(*tail));
	head->next = tail;
	tail->next = tail;
	head->path = NULL;
	tail->path = NULL;
	return (head);
}

void	delete_next_path(path_node *t)
{
	path_node	*temp;
	temp = t->next;
	t->next = t->next->next;
	free(temp->path);
    free(temp);
}

path_node	*insert_path_node(char *path, path_node *t)
{
	path_node	*new;

	new = malloc(sizeof(*new));
	new->path = strdup(path);
	new->next = t->next;
	t->next = new;
	return (new);
}

void	free_path_list(path_node *head)
{
	path_node	*current;
	path_node	*temp;

	current = head->next;
	free(head);
	while (current->next != current->next->next)
	{
		temp = current->next;
		free(current->path);
		free(current);
		current = temp;
	}
	temp = current->next;
	if (current->path != NULL)
	{
		free(current->path);
		free(current);
	}
	free(temp);
}

void	clear_path_list(path_node *head)
{
	while (head->next != head->next->next)
		delete_next_path(head);
}

int	check_path(path_node *copied, char *path)
{
	path_node	*current;

	if (copied->next == copied->next->next)
		return (0);
	current = copied->next;
	while (current != current->next)
	{
		if (strcmp(path, current->path) == 0)
			return(1);
		current = current->next;
	}
	return (0);
}

int	delete_path(char *path, path_node *head)
{
	path_node	*current;

	current = head;
	while (current->next != current->next->next)
	{
		if (strcmp(path, current->next->path) == 0)
		{
			delete_next_path(current);
			return (0);
		}
		current = current->next;
	}
	return (1);
}

char	*construct_path(char *buf, char *directory, char *file_name)
{
	if (my_strlen(directory) == 1 && *directory == '/')
	{
		sprintf(buf, "/%s", file_name);
	}
	else {
		sprintf(buf, "%s/%s", directory, file_name);
	}
	return (buf);
}

char	*get_file_name(char *path)
{
	int		i;
	char	*file_name;

	i = my_strlen(path) - 1;
	while (path[i] != '/')
	{
		i--;
	}
	file_name = strdup(&path[i + 1]);
	return (file_name);
}

int	check_file_exists(vd_node *dir_node, char *path)
{
	char	*file_name;
	entry_node *current = dir_node->directory->children->next;


	file_name = get_file_name(path);
	while (current != current->next)
	{
		if (strcmp(file_name, current->data->d_name) == 0)
		{
			free(file_name);
			return (1);
		}
		current = current->next;
	}
	free(file_name);
	return (0);
}

void	paste(vd_node *dir_node)
{
	path_node	*current;
	char		command_buf[500];
	char		*file_name;
	char		c;

	current = copied;
	while (current->next != current->next->next)
	{
		if (check_file_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", TERM_ROWS);
		}
		sprintf(command_buf, "cp -r \"%s\" %s", current->next->path, dir_node->dir_name);
		system(command_buf);
		memset(command_buf, 0, 500);
		delete_next_path(current);
	}
	current = cut;
	while (current->next != current->next->next)
	{
		if (check_file_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", TERM_ROWS, TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node));
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", TERM_ROWS);
		}
		file_name = get_file_name(current->next->path);
		construct_path(command_buf, dir_node->dir_name, file_name);
		rename(current->next->path, command_buf);
		memset(command_buf, 0, 500);
		free(file_name);
		delete_next_path(current);
	}
}

void	print_copied(path_node *copied)
{
	path_node *current = copied->next;
	printf("<addr: %p>\t<next: %p>\t<path: %s>\n",copied, copied->next, copied->path);
	while (current != current->next)
	{
		printf("<addr: %p>\t<next: %p>\t<path: %s>\n",current, current->next, current->path);
		current = current->next;
	}
	printf("<addr: %p>\t<next: %p>\t<path: %s>\n",current, current->next, current->path);
}

char	*user_from_uid(uid_t uid)
{
	size_t	size = 200;
	FILE	*fp;
	char	*line = malloc(size);
	char	*user;
	int		user_len;
	int		i;
	int		colons;

	fp = fopen("/etc/passwd", "r");
	while ((getline(&line, &size, fp)) != -1)
	{
		i = 0;
		colons = 0;
		while (colons < 2)
		{
			if (line[i++] == ':')
			{
				colons++;
				if (colons == 1)
					user_len = i;
			}
		}
		if ((int) uid == atoi(&line[i]))
			break;
	}
	user = malloc(user_len);
	strncpy(user, line, user_len - 1);
	user[user_len - 1] = 0;
	free(line);
	fclose(fp);
	return (user);
}

char	*group_from_gid(gid_t gid)
{
	size_t	size = 200;
	FILE	*fp;
	char	*line = malloc(size);
	char	*group;
	int		group_len;
	int		i;
	int		colons;

	fp = fopen("/etc/group", "r");
	while ((getline(&line, &size, fp)) != -1)
	{
		i = 0;
		colons = 0;
		while (colons < 2)
		{
			if (line[i++] == ':')
			{
				colons++;
				if (colons == 1)
					group_len = i;
			}
		}
		if ((int) gid == atoi(&line[i]))
			break;
	}
	group = malloc(group_len);
	strncpy(group, line, group_len - 1);
	group[group_len - 1] = 0;
	free(line);
	fclose(fp);
	return (group);
}

void	print_file_attributes(entry_node *entry)
{
	struct stat	fs;
	char		*user;
	char		*group;

	stat(entry->data->d_name, &fs);
	printf("%c", (S_ISDIR(fs.st_mode)) ? 'd' : '-');
	printf("%c", (fs.st_mode & S_IRUSR) ? 'r' : '-');
	printf("%c", (fs.st_mode & S_IWUSR) ? 'w' : '-');
	printf("%c", (fs.st_mode & S_IXUSR) ? 'x' : '-');
	printf("%c", (fs.st_mode & S_IRGRP) ? 'r' : '-');
	printf("%c", (fs.st_mode & S_IWGRP) ? 'w' : '-');
	printf("%c", (fs.st_mode & S_IXGRP) ? 'x' : '-');
	printf("%c", (fs.st_mode & S_IROTH) ? 'r' : '-');
	printf("%c", (fs.st_mode & S_IWOTH) ? 'w' : '-');
	printf("%c", (fs.st_mode & S_IXOTH) ? 'x' : '-');
	printf(" %ld", fs.st_size);
	user = user_from_uid(fs.st_uid);
	group = group_from_gid(fs.st_gid);
	printf(" %s", user);
	printf(" %s", group);

	free(user);
	free(group);
}
