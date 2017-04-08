
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

#include "../../st-revizor-watch.h"
#include "../../st-revizor-inline.h"
#include "../st-revizor-net.h"
#include "st-revizor-filter-url-debug.h"
#include "st-revizor-url.h"
#include "st-revizor-filter-url-tcp.h"

#define __URL_ERR_TCP_ENGINE 1
#include "st-revizor-filter-url-lang.h"

FUNEXPORT ssize_t tcp_socket_iserror(socket_select_e sidx, int iserr)
{
    switch(iserr)
    {
        case 0:
        {
            return ((sidx == 1) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND);
        }
        case EINTR:
#       if defined(EAGAIN)
        case EAGAIN:
#       elif defined(EWOULDBLOCK)
        case EWOULDBLOCK:
#       endif
        {
            return SOCK_CONTINUE;
        }
        case ETIMEDOUT:
        {
            return ((sidx == 1) ? SOCK_SRV_TIMEOUT : SOCK_CLI_TIMEOUT);
        }
        case ECONNRESET:
        {
            return ((sidx == 1) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
        }
        default:
        {
            return ((sidx == 1) ? SOCK_SRV_SHUT : SOCK_CLI_SHUT);
        }
    }
}

FUNEXPORT const char * tcp_socket_isdebug(socket_select_e sidx, int iserr)
{
#   if defined(DEBUG_URL_SENDRCV)
    if (sidx == SOCK_SERVER)
    {
        switch(iserr)
        {
            case SOCK_SRV_NEGATIVE:
            {
                return statusstrings[0];
            }
            case SOCK_SRV_DATAEND:
            {
                return statusstrings[1];
            }
            case SOCK_SRV_SHUT:
            {
                return statusstrings[2];
            }
            case SOCK_SRV_TIMEOUT:
            {
                return statusstrings[3];
            }
            case SOCK_CONTINUE:
            {
                return statusstrings[10];
            }
            case SOCK_ALL_OK:
            {
                return statusstrings[11];
            }
            default:
            {
                return statusstrings[4];
            }
        }
    }
    else if (sidx == SOCK_CLIENT)
    {
        switch(iserr)
        {
            case SOCK_CLI_NEGATIVE:
            {
                return statusstrings[5];
            }
            case SOCK_CLI_DATAEND:
            {
                return statusstrings[6];
            }
            case SOCK_CLI_SHUT:
            {
                return statusstrings[7];
            }
            case SOCK_CLI_TIMEOUT:
            {
                return statusstrings[8];
            }
            case SOCK_CONTINUE:
            {
                return statusstrings[10];
            }
            case SOCK_ALL_OK:
            {
                return statusstrings[11];
            }
            default:
            {
                return statusstrings[9];
            }
        }
    }
#   endif
    return ((iserr > 0) ? (const char*)&"no errors" : (const char*)&"unknown");
}

FUNEXPORT ssize_t tcp_recv(socket_select_e sidx, sa_socket_t *sa, char *rbuf, int bsz, int flags)
{
    ssize_t rsz = 0;
    time_t tms = (time(NULL) + (unsigned long)(HAVE_NET_TIMEOUT / 5 / 60));

    while (tms >= time(NULL))
    {
        errno = 0;
        if ((rsz = recv(sa->sp, rbuf, bsz, flags)) <= 0)
        {
            if (rsz < 0)
            {
#               if defined(EAGAIN)
                if ((errno == EAGAIN) || (errno == EINTR))
#               elif defined(EWOULDBLOCK)
                if ((errno == EWOULDBLOCK) || (errno == EINTR))
#               endif
                {
                    continue;
                }
                return tcp_socket_iserror(sidx, errno);
            }
            return ((rsz == 0) ?
                ((sidx == SOCK_SERVER) ?
                    ((errno == 0) ? SOCK_SRV_DATAEND : SOCK_SRV_SHUT) :
                    ((errno == 0) ? SOCK_CLI_DATAEND : SOCK_CLI_SHUT)) :
                ((sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE));
        }
        return rsz;
    }
    return ((sidx == SOCK_SERVER) ? SOCK_SRV_TIMEOUT : SOCK_CLI_TIMEOUT);
}
FUNEXPORT ssize_t tcp_recv_buffer(socket_select_e sidx, sa_socket_t *sa, buffer_u *buf, int flag)
{
    buf->sz = tcp_recv(sidx, sa, (char*)buf->str, __RBUF_SIZE, flag);
    return buf->sz;
}
FUNEXPORT ssize_t tcp_sends(socket_select_e sidx, sa_socket_t *sa, string_s *str)
{
    return tcp_send(sidx, sa, str->str, (ssize_t)str->sz);
}
FUNEXPORT ssize_t tcp_send(socket_select_e sidx, sa_socket_t *sa, char *rbuf, ssize_t rsz)
{
    if (sa->sp < 0)
    {
        return ((sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
    }

    ssize_t ssz = 0;
    time_t tms = (time(NULL) + (int)(HAVE_NET_TIMEOUT / 8 / 60));

    while ((ssz < rsz) && (tms >= time(NULL)))
    {
        errno = 0;
        char *pbuf = (char*)(rbuf + ssz);
        ssize_t sz = send(sa->sp, pbuf, (rsz - ssz), 0);
        if (sz < 0)
        {
#           if defined(EAGAIN)
            if ((errno == EAGAIN) || (errno == EINTR))
#           elif defined(EWOULDBLOCK)
            if ((errno == EWOULDBLOCK) || (errno == EINTR))
#           endif
            {
                continue;
            }
            return tcp_socket_iserror(sidx, errno);
        }
        else if (sz == 0)
        {
            return ((errno == 0) ?
                ((sidx == SOCK_SERVER) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND) :
                ((sidx == SOCK_SERVER) ? SOCK_SRV_SHUT    : SOCK_CLI_SHUT)
            );
        }
        ssz += sz;
    }
    return ((ssz == 0) ? ((sidx == SOCK_SERVER) ? SOCK_SRV_TIMEOUT : SOCK_CLI_TIMEOUT) : ssz);
}

#if defined(HAVE_SPLICE)

FUNEXPORT ssize_t tcp_transfer(srv_socket_set_t *sst)
{
    ssize_t ret = SOCK_ALL_OK;
    errno = 0;

    if (sst->from->sp < 0)
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
    }
    if ((ioctl(sst->from->sp, FIONREAD, &sst->rbytes) == 0) && (sst->rbytes == 0))
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND);
    }
    else if (sst->rbytes == 0)
    {
#       if defined(DEBUG_URL_SENDRCV)
        __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\tioctl FIONREAD error %s: (%s)",
            sst->tag, sst->ipstr->ip, sst->ipstr->port,
            ((sst->sidx == SOCK_SERVER) ? STR_URL_SRV : STR_URL_CLI),
            ((errno != 0) ? strerror(errno) : "-")
        );
#       endif
        return SOCK_IOCTLERR;
    }
    do
    {
        errno = 0;
        if ((sst->from->sp < 0) || (net_sock_iserror(sst->from->sp) != 0))
        {
            ret = ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
            break;
        }
        sst->rbytes = splice(
            sst->from->sp, NULL, sst->pipe[1], NULL, sst->bsz,
            SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK
        );
#       if defined(DEBUG_URL_SENDRCV)
        __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\t%s receive [%d] %s: %s (%s)",
            sst->tag, sst->ipstr->ip, sst->ipstr->port,
            ((sst->sidx == SOCK_SERVER) ? STR_URL_SRV : STR_URL_CLI),
            sst->rbytes,
            ((sst->rbytes > 0) ? "bytes" : "err code"),
            tcp_socket_isdebug(
                sst->sidx, sst->rbytes
            ),
            ((errno != 0) ? strerror(errno) : "-")
        );
#       endif
        if ((sst->rbytes < 0) && ((errno == EINTR) || (errno == EAGAIN)))
        {
            ret = SOCK_CONTINUE;
            break;
        }
        else if (sst->rbytes < 0)
        {
            ret = tcp_socket_iserror(sst->sidx, sst->rbytes);
            break;
        }
        else if (sst->rbytes == 0)
        {
            ret = ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND);
            break;
        }

        errno = 0;
        if ((sst->to->sp < 0) || (net_sock_iserror(sst->to->sp) != 0))
        {
            ret = ((sst->sidx == SOCK_SERVER) ? SOCK_CLI_NEGATIVE : SOCK_SRV_NEGATIVE);
            break;
        }
        sst->sbytes = splice(
            sst->pipe[0], NULL, sst->to->sp, NULL, sst->rbytes,
            SPLICE_F_MOVE | SPLICE_F_MORE | SPLICE_F_NONBLOCK
        );
#       if defined(DEBUG_URL_SENDRCV)
        __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\tSend to %s [%d] %s: %s (%s)",
            sst->tag, sst->ipstr->ip, sst->ipstr->port,
            ((sst->sidx == SOCK_SERVER) ? STR_URL_CLI : STR_URL_SRV),
            sst->sbytes,
            ((sst->sbytes > 0) ? "bytes" : "err code"),
            tcp_socket_isdebug(
                ((sst->sidx == SOCK_SERVER) ? SOCK_CLIENT : SOCK_SERVER), sst->sbytes
            ),
            ((errno != 0) ? strerror(errno) : "-")
        );
#       endif
        if ((sst->sbytes < 0) && (errno != EINTR) && (errno != EAGAIN))
        {
            ret = tcp_socket_iserror(
                ((sst->sidx == SOCK_SERVER) ? SOCK_CLIENT : SOCK_SERVER),
                sst->sbytes
            );
            break;
        }
        else if (sst->sbytes == 0)
        {
            ret = ((sst->sidx == SOCK_SERVER) ? SOCK_CLI_DATAEND : SOCK_SRV_DATAEND);
            break;
        }

        /* return 0 (SOCK_ALL_OK) if recive/send successful */
        return (int)(sst->rbytes - sst->sbytes);

    } while(0);

    return ret;
}

#else

FUNEXPORT ssize_t tcp_transfer(srv_socket_set_t *sst)
{
    errno = 0;

    if (sst->from->sp < 0)
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
    }
    if ((ioctl(sst->from->sp, FIONREAD, &sst->rbytes) == 0) && (sst->rbytes == 0))
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND);
    }
    else if (sst->rbytes == 0)
    {
#       if defined(DEBUG_URL_SENDRCV)
        __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\tioctl FIONREAD error %s: (%s)",
            sst->tag, sst->ipstr->ip, sst->ipstr->port,
            ((sst->sidx == SOCK_SERVER) ? STR_URL_SRV : STR_URL_CLI),
            ((errno != 0) ? strerror(errno) : "-")
        );
#       endif
        return SOCK_IOCTLERR;
    }

    char rbuf[__RBUF_SIZE] = {0};
    sst->bsz = ((sst->bsz > __RBUF_SIZE) ? __RBUF_SIZE : sst->bsz);

    errno = 0;
    if ((sst->from->sp < 0) || (net_sock_iserror(sst->from->sp) != 0))
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_NEGATIVE : SOCK_CLI_NEGATIVE);
    }

    sst->rbytes = tcp_recv(
        sst->sidx, sst->from, CP rbuf, sst->bsz, sst->flags
    );

#   if defined(DEBUG_URL_SENDRCV)
    __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\t%s receive [%d] %s: %s (%s)",
        sst->tag, sst->ipstr->ip, sst->ipstr->port,
        ((sst->sidx == SOCK_SERVER) ? STR_URL_SRV : STR_URL_CLI),
        sst->rbytes,
        ((sst->rbytes > 0) ? "bytes" : "err code"),
        tcp_socket_isdebug(
            sst->sidx, sst->rbytes
        ),
        ((errno != 0) ? strerror(errno) : "-")
    );
#   endif

    if (sst->rbytes < 0)
    {
        return sst->rbytes;
    }
    else if (sst->rbytes == 0)
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_SRV_DATAEND : SOCK_CLI_DATAEND);
    }

    errno = 0;
    if ((sst->to->sp < 0) || (net_sock_iserror(sst->to->sp) != 0))
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_CLI_NEGATIVE : SOCK_SRV_NEGATIVE);
    }

    sst->sbytes = tcp_send(
        ((sst->sidx == SOCK_SERVER) ? SOCK_CLIENT : SOCK_SERVER),
        sst->to, (char*)&(rbuf), sst->rbytes
    );

#   if defined(DEBUG_URL_SENDRCV)
    __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\tSend to %s [%d] %s: %s (%s)",
        sst->tag, sst->ipstr->ip, sst->ipstr->port,
        ((sst->sidx == SOCK_SERVER) ? STR_URL_CLI : STR_URL_SRV),
        sst->sbytes,
        ((sst->sbytes > 0) ? "bytes" : "err code"),
        tcp_socket_isdebug(
            ((sst->sidx == SOCK_SERVER) ? SOCK_CLIENT : SOCK_SERVER), sst->sbytes
        ),
        ((errno != 0) ? strerror(errno) : "-")
    );
#   endif

    if (sst->sbytes < 0)
    {
        return sst->rbytes;
    }
    else if (sst->sbytes == 0)
    {
        return ((sst->sidx == SOCK_SERVER) ? SOCK_CLI_DATAEND : SOCK_SRV_DATAEND);
    }

    return (int)(sst->rbytes - sst->sbytes);
}
#endif
