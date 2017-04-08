//--------------------------------------------------------------------------------------
//
//    stRevizor package - Registry RosKomNadzor blacklist converter to iptables/ipset,
//              Win GUI client, *nix server part and more..
//
//    stRevizor server - Registry RosKomNadzor blacklist watch daemon.
//              Watch FTP/TFTP directory for the presence of RosKomNadzor
//              registry update files and makes changes to the iptables/ipset.
//
//              Copyright (C) 2015-2017, @PS
//
//    Everyone is permitted to copy and distribute verbatim copies
//    of this license document, but changing it is not allowed.
//
//    See file LICENSE.md in directory distribution.
//
//    You can find latest source:
//     - https://github.com/PetersSharp/stRevizor
//     - https://github.com/PetersSharp/stRevizor/releases/latest
//     - https://github.com/PetersSharp/stRevizor/pulse
//
//    * $Id$
//    * commit date: $Format:%cd by %aN$
//
//--------------------------------------------------------------------------------------

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include "../st-revizor-watch.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "st-revizor-net.h"

#if !defined(HAVE_WATCH_DAEMON) || (HAVE_WATCH_DAEMON != 1)

#if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
    (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
FUNEXPORT void net_poll_print(struct pollfd *pfd, char *file, int line)
{
    if (file != NULL)
    {
        __MACRO_TOSCREEN__(
            "\t* Debug pollfd -> locations: %s:%d",
            file, line
        );
    }
    char *pollev = NULL;

    if (0)                               { ; }
#   if defined(POLLHUP)
    else if(pfd->revents & POLLHUP)      { pollev = "POLLHUP";  }
#   endif
#   if defined(POLLERR)
    else if(pfd->revents & POLLERR)      { pollev = "POLLERR";  }
#   endif
#   if defined(POLLIN)
    else if(pfd->revents & POLLIN)       { pollev = "POLLIN";  }
#   endif
#   if defined(POLLRDNORM)
    else if(pfd->revents & POLLRDNORM)   { pollev = "POLLRDNORM (POLLIN)";  }
#   endif
#   if defined(POLLNVAL)
    else if(pfd->revents & POLLNVAL)     { pollev = "POLLNVAL";  }
#   endif
#   if defined(POLLPRI)
    else if(pfd->revents & POLLPRI)      { pollev = "POLLPRI";  }
#   endif
#   if defined(POLLWRBAND)
    else if(pfd->revents & POLLWRBAND)   { pollev = "POLLWRBAND";  }
#   endif
#   if defined(POLLRDHUP)
    else if(pfd->revents & POLLRDHUP)    { pollev = "POLLRDHUP";  }
#   endif
#   if defined(POLLOUT)
    else if(pfd->revents & POLLOUT)      { pollev = "POLLOUT";  }
#   endif
#   if defined(POLLWRNORM)
    else if(pfd->revents & POLLWRNORM)   { pollev = "POLLWRNORM (POLLOUT)";  }
#   endif

    __MACRO_TOSCREEN__(
        "\t\tfd:\t\t%d\n" \
        "\t\tevent state:\t%s\n" \
        "\t\tevent watch:\t%u\n",
        pfd->fd, pollev, pfd->events
    );
}
#endif
FUNEXPORT void net_sa_print(sa_socket_t *sa, char *name, char *file, int line)
{
    if (file != NULL)
    {
        __MACRO_TOSCREEN__(
            "\t* Debug sa_socket_t (%s) -> locations: %s:%d",
            ((name != NULL) ? name : "-noname-"),
            file, line
        );
    }
    __MACRO_TOSCREEN__(
        "\t\tport (sp):\t%d\n" \
        "\t\tsize (sl):\t%u\n" \
        "\t\tfamily (sa):\t%d\n" \
        "\t\taddr (sa):\t%s:%u",
        sa->sp, sa->sl, sa->sa.sin_family,
        inet_ntoa(sa->sa.sin_addr), ntohs(sa->sa.sin_port)
    );
}
FUNEXPORT void net_proxy_print(sa_proxy_t *sap, int queue, char *file, int line)
{
    if (file != NULL)
    {
        __MACRO_TOSCREEN__(
            "\t* Debug sa_proxy_t, queue idx %d -> locations: %s:%d",
            queue, file, line
        );
    }
    __MACRO_TOSCREEN__("\t- client socket (%s):", "sac");
    net_sa_print((sa_socket_t*)&(sap->sac), NULL, NULL, 0);
    __MACRO_TOSCREEN__("\t- server socket (%s):", "sas");
    net_sa_print((sa_socket_t*)&(sap->sas), NULL, NULL, 0);
    __MACRO_TOSCREEN__("\t- destination socket (%s):", "sad");
    net_sa_print((sa_socket_t*)&(sap->sad), NULL, NULL, 0);

#   if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
       (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
    if (sap->pfd != NULL)
    {
        __MACRO_TOSCREEN__("\t- pollfd (%s):", "pfd");
        net_poll_print(sap->pfd, NULL, 0);
    }
    else
    {
        __MACRO_TOSCREEN__("\t- pollfd (%s) not set..", "pfd");
    }
    __MACRO_TOSCREEN__(
        "\t\tbuffer size:\t%d\n", sap->tcpbs
    );
#   endif
}
#   endif
