/*
 * Copyright © 2012 Jason J.A. Stephenson <jason@sigio.com>
 *
 * This file is part of wake.
 *
 * wake is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * wake is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wake.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "hostinfo.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

/*
 * Reads the file at path looking for a list of hostname mac address
 * pairs on the lines, and returns a list of struct hostinfo pointers
 * to the data found.
 *
 * Lines with valid hostname and mac address entries should begin with
 * the hostname, followed by the mac address separated by 1 or more
 * whitespace characters. The bytes of the mac address must be
 * separated by some puncutation character.
 *
 * Lines that begin with a pound sign (#) character or with blank
 * space are ignored, so you can use either to begin comments.
 */
list_t *parse_wake_hosts_file(char *path)
{
	/* parse a hosts file and ready the info into a list. */
	list_t *head = NULL;
	struct hostinfo *curhost;
	char inbuf[128];
	int i, j, error = 0;

	FILE *infile;

	infile = fopen(path, "r");
	if (infile) {
		while (fgets(inbuf, 128, infile) != NULL) {
			i = 0;
			if (inbuf[i] == '#') continue;
			while (!isspace(inbuf[i]))
				i++;
			if (i > 0) { /* Skip lines that begin with whitespace */
				curhost = malloc(sizeof(struct hostinfo));
				if (curhost == NULL) {
					error = errno;
					if (head != NULL)
						free_wake_hosts_list(head);
					break;
				}
				curhost->name = calloc(i + 1, sizeof(char));
				if (curhost->name == NULL) {
					error = errno;
					if (head != NULL)
						free_wake_hosts_list(head);
					free(curhost);
					break;
				}
				strncpy(curhost->name, inbuf, i);
				curhost->name[i] = '\0';
				curhost->macaddr = calloc(18, sizeof(char));
				if (curhost->macaddr == NULL) {
					error = errno;
					if (head != NULL) 
						free_wake_hosts_list(head);
					free(curhost->name);
					free(curhost);
					break;
				}
				j = 0;
				while (isspace(inbuf[i]))
					i++;
				while (!isspace(inbuf[i+j])) {
					j++;
					if (j == 17) break;
				}
				if (i < strlen(inbuf) && j > 0) {
					strncpy(curhost->macaddr, (inbuf + i), j);
					curhost->macaddr[j] = '\0';
					if (head == NULL) {
						head = initialize_list(1, (void *) curhost);
						if (head == NULL) {
							error = errno;
							free(curhost->name);
							free(curhost->macaddr);
							free(curhost);
							break;
						}
					}
					else {
						if (append_list_data(head, (void *) curhost) == NULL) {
							error = errno;
							free_wake_hosts_list(head);
							free(curhost->name);
							free(curhost->macaddr);
							free(curhost);
							break;
						}
					}
				}
			}
		}
		fclose(infile);
		if (error)
			errno = error;
	}
	return head;
}

/*
 * Frees all the memory allocated for a list of struct hostinfo objects.
 */
void free_wake_hosts_list(list_t *list)
{
	list_t *current = rewind_list(list);
	struct hostinfo *curhost;
	while (current != NULL) {
		curhost = (struct hostinfo *) current->data;
		if (curhost != NULL) {
			if (curhost->name != NULL)
				free(curhost->name);
			if (curhost->macaddr != NULL)
				free(curhost->macaddr);
			free(curhost);
		}
		current = current->next;
	}
	free_list(list);
	list = NULL;
}

/*
 * Compares two hostinfo pointers based on their names using strcasecmp.
 * This function is suitable for sorting or searching a list of struct
 * hostinfo pointers.
 */
int hostcasecmpname(struct hostinfo *a, struct hostinfo *b)
{
	return strcasecmp(a->name, b->name);
}

/*
 * Searches the list pointed at by list for the first struct hostinfo node that
 * matches the name parameter. It returns the matching struct hostinfo pointer
 * or NULL if nothing matches the name.
 */
struct hostinfo *find_host_by_name(list_t *list, char *name)
{
	struct hostinfo des;
	des.name = name;
	des.macaddr = NULL;

	list_t *found = search_list(list, &des,
		(int (*)(void *, void *))hostcasecmpname);
	if (found != NULL)
		return (struct hostinfo *) found->data;
	return NULL;
}

/*
 * Searches $home/wake.hosts, /etc/wake.hosts and finally ./wake.hosts
 * until a file is found. Return a pointer to the full path to the
 * found file or NULL if the file is not found. The memory to hold the
 * value of the return pointer is static and so should not be freed.
 */
char *find_wake_hosts_file_path(void)
{
	static char fname[FILENAME_MAX];
	extern int errno;

	const char *file = "wake.hosts";

	struct stat sb;
	struct passwd *pwent;
	uid_t id;

	char *home;

	errno = 0;

	/* Nul the fname buffer each time we're called. */
	memset(fname, '\0', FILENAME_MAX);

	home = getenv("HOME");
	if (home != NULL) {
		if (strlen(home) < FILENAME_MAX) {
			strncpy(fname, home, strlen(home));
			if (fname[strlen(home) - 1] != '/')
				strcat(fname, "/");
			if (strlen(fname) + strlen(file) < FILENAME_MAX) {
				strcat(fname, file);
				if (stat(fname, &sb) == 0)
					return fname;
				else if (errno != ENOENT)
					return NULL;
			}
		}
	}
	else {
		id = getuid();
		pwent = getpwuid(id);
		if (pwent != NULL) {
			home = pwent->pw_dir;
			if (home != NULL) {
				if (strlen(home) < FILENAME_MAX) {
					strncpy(fname, home, strlen(home));
					if (fname[strlen(home) - 1] != '/')
						strcat(fname, "/");
					if (strlen(fname) + strlen(file) < FILENAME_MAX) {
						strcat(fname, file);
						if (stat(fname, &sb) == 0)
							return fname;
						else if (errno != ENOENT)
							return NULL;
					}
				}
			}
		}
	}

	if (strlen(fname))
		memset(fname, '\0', strlen(fname));

	/* We made it to here, so we'll check SYSCONFDIR. */
	strcpy(fname, SYSCONFDIR);
	if (fname[strlen(SYSCONFDIR) - 1] != '/')
		strcat(fname, "/");
	strcat(fname, file);
	if (stat(fname, &sb) == 0)
		return fname;
	else if (errno != ENOENT && errno != EACCES)
		return NULL;

	/* We made it to here, so we check the current directory. */
	strcpy(fname, file);
	if (stat(fname, &sb) == 0)
		return fname;

	return NULL;
}
