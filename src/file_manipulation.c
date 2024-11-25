#include "headers/structs.h"
#include "headers/format.h"
#include "headers/utils.h"
#include "headers/env.h"
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

path_node	*copied;
path_node	*cut;

path_node	*init_path_list(void)
// Initialises a linked list of paths.
// Returns:
//  - Pointer to head of path list
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
// Deletes the next node from a given path list.
// Args:
//  - t:	pointer to node before the node to be deleted
{
	path_node	*temp;
	temp = t->next;
	t->next = t->next->next;
	free(temp->path);
	free(temp);
}

path_node	*insert_path_node(char *path, path_node *t)
// Inserts a node into a path list.
// Args:
//  - path:	the path to be inserted into the list
//  - t:	pointer to the node after which the new node will be inserted
// Returns:
//  - Pointer to the newly inserted path node
{
	path_node	*new;

	new = malloc(sizeof(*new));
	new->path = strdup(path);
	new->next = t->next;
	t->next = new;
	return (new);
}

void	free_path_list(path_node *head)
// Frees a given path list.
// Args:
//  - head:	pointer to the head of the path list
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
// Removes all members of a given path list.
// Args:
//  - head:	pointer to the head of the path list
{
	while (head->next != head->next->next)
		delete_next_path(head);
}

int	check_path(path_node *path_list, char *path)
// Checks if a path is in a given path list.
// Args:
//  - path_list:	pointer to the head of the path list being checked
//  - path:			path to be checked
// Returns:
//  - 1 if path is in path list
//  - 0 if path is not in path list
{
	path_node	*current;

	if (path_list->next == path_list->next->next)
		return (0);
	current = path_list->next;
	while (current != current->next)
	{
		if (strcmp(path, current->path) == 0)
			return(1);
		current = current->next;
	}
	return (0);
}

int	delete_path(char *path, path_node *head)
// Searches for a path in a given path list, and if found
// deletes path from the list.
// Args:
//  - path:	path to be deleted
//  - head:	pointer to head of path list to search
// Returns:
//  - 0 if path successfully found and deleted
//  - 1 if path not found in list
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
// Constructs an absolute path from a file name and directory path.
// Args:
//  - buf:			char array where the path will be constructed
//  - directory:	full path of directory containing file
//  - file_name:	name of the file
// Returns:
//  - Pointer to array where path was constructed
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
// Gets the file name from an absolute path.
// Args:
//  - path:	absolute path of file
// Returns:
//  - Pointer to memory allocated string containing file name
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

int	check_path_exists(vd_node *dir_node, char *path)
// Checks if a file with a given absolute path exists within a directory.
// Args:
//  - dir_node:	pointer to node of directory to check
//  - path:		absolute path of file to check
// Returns:
//  - 1 if file exists in directory
//  - 0 if file doesn not exist in directory
{
	char		*file_name;
	entry_node	*current = dir_node->directory->children->next;


	file_name = get_file_name(path);
	while (current != current->next)
	{
		if (strcmp(file_name, current->d_name) == 0)
		{
			free(file_name);
			return (1);
		}
		current = current->next;
	}
	free(file_name);
	return (0);
}

int	check_file_exists(vd_node *dir_node, char *file_name)
// Checks if a file with a given file name exists within a directory.
// Args:
//  - dir_node:	pointer to node of directory to check
//  - path:		name of file to check
// Returns:
//  - 1 if file exists in directory
//  - 0 if file doesn not exist in directory
{
	entry_node	*current = dir_node->directory->children->next;

	while (current != current->next)
	{
		if (strcmp(file_name, current->d_name) == 0)
			return (1);
		current = current->next;
	}
	return (0);
}


void	paste(vd_node *dir_node)
// Copies all files from 'copied' list into given directory.
// Moves all files from 'cut' list into given directory.
// If a file already exists with the same name, asks user what action
// should be taken.
// Removes all pasted files from their respective lists.
// Args:
//  - dir_node:	pointer to node of directory to paste in
{
	path_node	*current;
	char		command_buf[500];
	char		*file_name;
	char		c;

	current = copied;
	while (current->next != current->next->next)
	{
		if (check_path_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", env.TERM_ROWS, env.TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node), 0);
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node), 0);
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", env.TERM_ROWS);
		}
		sprintf(command_buf, "cp -r \"%s\" %s", current->next->path, dir_node->dir_name);
		system(command_buf);
		memset(command_buf, 0, 500);
		delete_next_path(current);
	}
	current = cut;
	while (current->next != current->next->next)
	{
		if (check_path_exists(dir_node, current->next->path))
		{
			printf("\e[%d;3H[ \e[1;33mFile exists. Overwrite? [y/N] : \e[m%.*s ]", env.TERM_ROWS, env.TERM_COLS - 38, current->next->path);
			if ((c = getchar()) != 'y')
			{
				current = current->next;
				draw_box();
				display_directory(dir_node, get_selected(dir_node), get_parent(dir_node), 0);
				continue;
			}
			draw_box();
			display_directory(dir_node, get_selected(dir_node), get_parent(dir_node), 0);
			printf("\e[%d;3H[ \e[1;33mFile overwritten!\e[m ]", env.TERM_ROWS);
		}
		file_name = get_file_name(current->next->path);
		construct_path(command_buf, dir_node->dir_name, file_name);
		rename(current->next->path, command_buf);
		memset(command_buf, 0, 500);
		free(file_name);
		delete_next_path(current);
	}
}

char	*user_from_uid(uid_t uid)
// Retrieves the username of a given uid.
// Args:
//  - uid:	uid of user
// Returns:
//  - memory allocated string containing username
{
	size_t	size = 200;
	FILE	*fp;
	char	*line = malloc(size);
	char	*user;
	int		user_len = 0;
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
// Retrieves the groupname of a given gid.
// Args:
//  - gid:	gid of group
// Returns:
//  - memory allocated string containing groupname
{
	size_t	size = 200;
	FILE	*fp;
	char	*line = malloc(size);
	char	*group;
	int		group_len = 0;
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

void	print_time(time_t time)
// Prints formatted time.
// Args:
//  - time:	the time to print
{
	struct tm	*timeinfo;
	char 		*time_str;
	int 		i;
	timeinfo = localtime(&time);
	time_str = asctime(timeinfo);
	i = 0;
	while (time_str[i] != 0)
	{
		if (time_str[i] == '\n')
			time_str[i] = 0;
		i++;
	}
	printf(" %s", time_str);
}

void	format_filesize(off_t filesize)
// Prints a filesize formatted with appropriate SI prefixes.
// Args:
//  - filesize:	the filesize to print
{
	double	size = (double) filesize;
	int		power = 0;
	
	while (size >= 1000)
	{
		size /= 1000;
		power++;
	}
	switch (power) {
		case (1):
			if (size < 10)
				printf("\e[1;32m%3.1fK", size);
			else
				printf("\e[1;32m%3.0fK", size);
			break;
		case (2):
			if (size < 10)
				printf("\e[33m%3.1fM", size);
			else
				printf("\e[33m%3.0fM", size);
			break;
		case (3):
			if (size < 10)
				printf("\e[31m%3.1fG", size);
			else
				printf("\e[31m%3.0fG", size);
			break;
		case (4):
			if (size < 10)
				printf("\e[38m%3.1fT", size);
			else
				printf("\e[38m%3.0fT", size);
			break;
		default:
			printf("\e[32m%4.0f", size);
			break;
	}
	printf("\e[m");
}

void	print_file_attributes(entry_node *entry)
// Prints the following attributes of a file:
//  - file permissions
//  - (formatted) filesize
//  - username of file owner
//  - groupname of file group
//  - modified time of file
// Args:
//  - entry:	pointer to entry node of the file
{
	char		*user;
	char		*group;

	if (entry == NULL)
		return ;
	if (entry->attr == NULL)
		return ;
	switch (entry->d_type) {
		case (DT_LNK):
			printf("\e[1;36ml");
			break;
		case (DT_DIR):
			printf("\e[1;34md");
			break;
		default:
			printf("\e[1;30m-");
			break;
	}
	printf("%s", (entry->attr->st_mode & S_IRUSR) ? "\e[33mr" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IWUSR) ? "\e[31mw" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IXUSR) ? "\e[32mx" : "\e[30m-");
	printf("\e[m%s", (entry->attr->st_mode & S_IRGRP) ? "\e[33mr" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IWGRP) ? "\e[31mw" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IXGRP) ? "\e[32mx" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IROTH) ? "\e[33mr" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IWOTH) ? "\e[31mw" : "\e[30m-");
	printf("%s", (entry->attr->st_mode & S_IXOTH) ? "\e[32mx" : "\e[30m-");
	printf(" \e[m");
	(entry->d_type == DT_REG) ? format_filesize(entry->attr->st_size) : printf("\e[30m   -");
	user = user_from_uid(entry->attr->st_uid);
	group = group_from_gid(entry->attr->st_gid);
	printf(" \e[1;33m%s", user);
	printf(" \e[1;34m%s", group);
	printf("\e[0;35m");
	print_time(entry->attr->st_mtime);
	printf("\e[m");

	free(user);
	free(group);
}


void	recursive_dir_size(vd_node *dir_node, size_t *total, char **buf)
{
	vd_node		*child;
	entry_node	*current;

	cleanup_directory(dir_node);
	if (get_directory(dir_node) == NULL)
		return ;
	current = dir_node->directory->children->next;
	while (current != current->next)
	{
		if (current->d_type == DT_DIR)
		{
			if (strcmp(current->d_name, ".") != 0 && strcmp(current->d_name, "..") != 0)
			{
				construct_path(*buf, dir_node->dir_name, current->d_name);
				child = get_vd_node(VISITED_DIRS, *buf);
				recursive_dir_size(child, total, buf);
			}
		}
		else if (current->d_type == DT_REG)
		{
			if (current->attr != NULL)
				*total += current->attr->st_size;
		}
		current = current->next;
	}
	cleanup_directory(dir_node);
}

size_t	get_dir_size(vd_node *dir_node)
{
	size_t	*total = malloc(sizeof(*total));
	char	*buf = malloc(500);
	size_t	output;
	int		hidden = 0;

	*total = 0;
	if (env.FLAGS & F_HIDDEN)
		hidden = 1;
	else
		env.FLAGS ^= F_HIDDEN;
	recursive_dir_size(dir_node, total, &buf);
	output = *total;
	free(total);
	free(buf);
	if (hidden == 0)
		env.FLAGS ^= F_HIDDEN;
	return (output);
}
