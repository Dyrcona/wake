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
#ifndef BROADCAST_INCL
#define BROADCAST_INCL 1

#include <sys/types.h>

ssize_t
broadcast_msg(const u_int16_t port, const char *msg, const size_t msglen);

char *
build_msg(char *macaddr, char *msgbuf);

#endif
