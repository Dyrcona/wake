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
#include "list.h"
#include "broadcast.h"

#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  extern int errno;
  char magic[102];
  int i = 0;

  list_t *head;
  struct hostinfo *curhost;

  /* Some regex stuff to validate the input. */
  regex_t regexp;
  char *repat = "^([[:xdigit:]]{1,2}([[:punct:]]|$)){6}";
  int reflags = REG_EXTENDED | REG_NOSUB;

  /* Look up file location. */
  char *hostsfname = find_wake_hosts_file_path();
  if (hostsfname == NULL) {
    fprintf(stderr, "Can't find wake.hosts file\n");
    exit(errno);
  }

  head  = parse_wake_hosts_file(hostsfname);
  if (head == NULL) {
    fprintf(stderr, "Can't parse file %s: %s\n", hostsfname,
      strerror(errno));
    exit(errno);
  }

  if (regcomp(&regexp, repat, reflags) == 0) {
    for (i = 1; i < argc; i++) {
      curhost = find_host_by_name(head, argv[i]);
      if (curhost == NULL) {
        fprintf(stderr, "Host not found in %s: %s\n", hostsfname,
          argv[i]);
        continue;
      }

      if (regexec(&regexp, curhost->macaddr, 0, NULL, 0) == REG_NOMATCH)
        fprintf(stderr, "Invalid mac address (%s) for host (%s).\n",
          curhost->macaddr, curhost->name);
      else
        if (build_msg(curhost->macaddr, magic) != NULL) {
          if (broadcast_msg(9, magic, 102) == -1)
            fprintf(stderr, "Unable to send broadcast: %s\n",
              strerror(errno));
        }
        else
          fprintf(stderr, "Failed to build magic packet for %s.\n",
            curhost->name);
    }
    regfree(&regexp);
  }
  free_wake_hosts_list(head);

  return 0;
}
