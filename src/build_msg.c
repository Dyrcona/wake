/*
 * Copyright © 2016 Jason J.A. Stephenson <jason@sigio.com>
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
#include "build_msg.h"
#include <string.h>
#include <stdlib.h>

/*
 * Builds the magic packet for the mac address in the first argument.
 *
 * The magic packet message is returned in msgbuf which must be able
 * to hold 102 bytes, which is the space required for the magic
 * packet. The msgbuf is not nul-terminated (as it may contain nul
 * characters) and no checking is done of the space available in the
 * destination pointer.
 *
 * Returns a pointer to msgbuf on success or NULL on failure.
 */
char *
build_msg(char *macaddr, char *msgbuf)
{
  char *next; /* Used in parsing the macaddr with strol. */
  int i; /* a loop iterator */

  /* The first 6 bytes of the magic packet should have the value of 0xFF. */
  memset(msgbuf, 0xFF, 6);

  /* The remaining 96 bytes of the magic packet are the mac address
   * of the destination computer repeated 16 times. The mac address
   * is stuffed into a string of 6 characters with each character
   * having the numeric value of that byte from the mac address. */

  for (i = 6; i < 102; i++) {
    if (i % 6 == 0)
      next = macaddr;
    msgbuf[i] = (char)strtol(macaddr + (next - macaddr), &next, 16);
    next++;
  }

  return msgbuf;
}

