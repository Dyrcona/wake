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
#ifndef HOSTINFO_INCL
#define HOSTINFO_INCL 1

#include "list.h"

#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif

/* Used to hold hostname/mac address pairs read from the file. */
struct hostinfo {
  char *name;
  char *macaddr;
};

char *find_wake_hosts_file_path(void);

list_t *parse_wake_hosts_file(char *path);

void free_wake_hosts_list(list_t *list);

int hostcasecmpname(struct hostinfo *a, struct hostinfo *b);

struct hostinfo *find_host_by_name(list_t *list, char *name);

#endif
