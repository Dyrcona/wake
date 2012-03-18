# Copyright © 2012 Jason J.A. Stephenson <jason@sigio.com>
#
# This file is part of wake.
#
# wake is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# wake is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with wake.  If not, see <http://www.gnu.org/licenses/>.

ifdef DEBUG
CFLAGS += -g
else
.INTERMEDIATE: list.o broadcast.o hostinfo.o wake.o
endif

VPATH = src

wake: wake.o list.o hostinfo.o broadcast.o
	$(LINK.c) $^ $(LOADLIBES) -o $@ $(LDLIBS)

wake.o: list.h broadcast.h hostinfo.h wake.c

list.o: list.h list.c

broadcast.o: broadcast.h broadcast.c

hostinfo.o: list.h hostinfo.h hostinfo.c
