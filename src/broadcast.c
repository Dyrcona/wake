/*
 * Copyright © 2012,2016 Jason J.A. Stephenson <jason@sigio.com>
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
#include "broadcast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <errno.h>

/*
 * Broadcasts a UDP msg to all interfaces, except the loopback
 * interface.  Since IPv6 doesn't support broadcast, this only works
 * with IPv4.
 *
 * Returns the number of bytes broadcast or -1 on error.
 */
ssize_t
broadcast_msg(const u_int16_t port, const char *msg, const size_t msglen)
{
  ssize_t rv = -1; /* Assume an error as this simplifies things below. */
  const int on = 1;
  extern int errno;
  int lastlen = 0; /* last length returned with SIOCGIFCONF below */
  int n = 30; /* number of interfaces? */
  void *t; /* temp for realloc */
  char lastname[IFNAMSIZ], *cptr; /* track interface names */

  struct ifconf ifc;
  struct ifreq *ifr, ifrcopy;
  struct sockaddr_in sa, *sin;

  /* Zero out our structs to clear up any garbage. */
  memset(&ifc, 0, sizeof(struct ifconf));
  memset(&sa, 0, sizeof(struct sockaddr_in));

  int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd != -1) {
    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    /*
     * Find the available network interfaces.  We look for an
     * arbitrary number of interfaces, starting with 30.  We keep
     * going until ioctl returns OK and the size returned does not
     * change.
     */
    for (;;) {
      /* Setup our struct ifconf. */
      ifc.ifc_len = sizeof(struct ifreq) * n;
      t = realloc(ifc.ifc_buf, ifc.ifc_len);
      if (t)
        ifc.ifc_buf = t;
      else {
#ifdef DEBUG
        fprintf(stderr, "Allocating buffer\n");
#endif
        fprintf(stderr, "%s\n", strerror(errno));
        goto CLEAN_UP;
      }

      /* Request the interface configurations */
      if (ioctl(sock_fd, SIOCGIFCONF, &ifc) == -1) {
        /* 'Cause Solaris sets errno to EINVAL. */
        if (errno != EINVAL || lastlen != 0) {
#ifdef DEBUG
          fprintf(stderr, "Getting configuration");
#endif
          fprintf(stderr, "%s\n", strerror(errno));
          goto CLEAN_UP;
        }
      }
      else if (ifc.ifc_len == lastlen)
        break;
      /*
       * We use lastlen because BSD-derived implementations may return
       * a short len if another struct would not fit.
       */
      lastlen = ifc.ifc_len;
      n += 10;
    }

    /*
     * Loop through the interfaces and send our broadcast on each one,
     * skipping the loopback interface.
     */
    for (t = ifc.ifc_buf; t < (void *)ifc.ifc_buf + ifc.ifc_len;) {
      /* Get the interface. */
      ifr = (struct ifreq *) t;
      /* Make a copy of it. */
      ifrcopy = *ifr;
      /* get the next one */
      t += sizeof(struct ifreq);
      /* Check for aliases. */
      if ((cptr = strchr(ifr->ifr_name, ':')) != NULL)
        *cptr = 0; /* replace colon with nul */
      if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0)
        continue; /* Skip if we've seen this name before. */
      /* Store the name if not. */
      memcpy(lastname, ifr->ifr_name, IFNAMSIZ);
      /* Get the interface flags. */
      if (ioctl(sock_fd, SIOCGIFFLAGS, &ifrcopy) == -1) {
#ifdef DEBUG
        fprintf(stderr, "Getting flags\n");
#endif
        fprintf(stderr, "%s\n", strerror(errno));
        goto CLEAN_UP;
      }
      /* Skip the interface if it is not up. */
      if ((ifrcopy.ifr_flags & IFF_UP) == 0)
        continue;
      /* Skip the interface if it is the loopback interface. */
      if ((ifrcopy.ifr_flags & IFF_LOOPBACK))
        continue;
      /* Skip the interface if the broadcast flag is not set. */
      if ((ifrcopy.ifr_flags & IFF_BROADCAST) == 0)
        continue;
      if (ioctl(sock_fd, SIOCGIFBRDADDR, ifr) != -1) {
        if (ifr->ifr_broadaddr.sa_family == AF_INET) {
          sin = (struct sockaddr_in*) &ifr->ifr_broadaddr;
          sa.sin_addr.s_addr = sin->sin_addr.s_addr;
          rv = sendto(sock_fd, msg, msglen, 0,
            (const struct sockaddr *) &sa,
            sizeof(struct sockaddr_in)
          );
        }
      } else {
#ifdef DEBUG
        fprintf(stderr, "Sending broadcast\n");
#endif
        fprintf(stderr, "%s\n", strerror(errno));
        goto CLEAN_UP;
      }
    }
  }
  else
    fprintf(stderr, "%s\n", strerror(errno));

CLEAN_UP:
  if (sock_fd != -1)
    close(sock_fd);
  if (ifc.ifc_buf) {
    free(ifc.ifc_buf);
    ifc.ifc_buf = NULL;
  }

  return rv;
}
