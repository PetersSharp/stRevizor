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
#include "st-revizor-url.h"
#include "st-revizor-filter-url-debug.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../../util/thpool.h"

extern int isrun;

#if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
    (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
static void net_close_(struct pollfd *pfd)
{
    shutdown(pfd->fd,SHUT_RDWR);
    close(pfd->fd);
    pfd->fd = -1;
    pfd->revents = 0;
}

#elif (defined(HAVE_SELECT_H) && (HAVE_SELECT_H == 1)) || \
      (defined(HAVE_SYS_SELECT_H) && (HAVE_SYS_SELECT_H == 1))

static void __net_close_(int *sock)
{
    shutdown(*sock, SHUT_RDWR);
    close(*sock);
    sock = -1;
}
static void net_close_(sa_proxy_t *sap)
{
    if (sap->sac.sp > -1) { __net_close_(&sap->sac.sp); }
    if (sap->sas.sp > -1) { __net_close_(&sap->sas.sp); }
}
#endif

static void srv_poll_default_(srv_socket_url_t *srv, int idx)
{
    net_proxy_default(&srv->sa2[idx], NULL, &srv->sa2[0].sas, NULL);
    srv->pfd[idx].fd      = -1;
    srv->pfd[idx].revents =  0;
    srv->pfd[idx].events  =  0;
}
FUNEXPORT void net_poll_session_end(sa_proxy_t *sa2, int port)
{
    if (port > 0)
    {
        shutdown(port, SHUT_RDWR);
        close(port);
    }
    else if ((sa2 != NULL) && (sa2->pfd->fd != -1))
    {
        shutdown(sa2->pfd->fd,SHUT_RDWR);
        close(sa2->pfd->fd);
    }
    if (sa2 != NULL)
    {
        sa2->pfd->fd      = -1;
        sa2->pfd->revents =  0;
        sa2->pfd->events  =  0;
        net_proxy_default(sa2, NULL, NULL, NULL);
    }
}
FUNEXPORT int net_slot_find(srv_socket_url_t *srv)
{
    int i;
    __TABLE_RLOCK(srv->reqlock,
        for (i = 1; i < URL_NET_QUEUE; i++)
        {
            if (srv->pfd[i].fd == -1)
            {
                break;
            }
        }
    );
    if (i == (URL_NET_QUEUE - 1))
    {
        return -1;
    }
    return i;
}
FUNEXPORT int net_slot_add(srv_socket_url_t *srv, sa_proxy_t *sai, int port)
{
    int idx;

    if ((idx = net_slot_find(srv)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "URL event poll slot", "max user");
        net_poll_session_end(srv->spt, port);
        return -1;
    }

    struct pollfd nfd = {0};
    nfd.fd      = port;
    nfd.events  = POLLIN | POLLERR | POLLHUP;
    nfd.revents = 0;

    __TABLE_WLOCK(srv->reqlock,
        memcpy((void*)&srv->pfd[idx], (void*)&nfd, sizeof(struct pollfd));
        memcpy((void*)&srv->sa2[idx], (void*)sai, sizeof(sa_proxy_t));
        srv->sa2[idx].pfd = (struct pollfd*)&srv->pfd[idx];
    );
    return port;
}
FUNEXPORT int url_event_poll(srv_transport_t *tr)
{
    if (
        (tr == NULL) || (tr->sa2 == NULL) ||
        (tr->tcpreg_q == NULL) || (tr->tcpreg_r == NULL)
       )
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "URL event poll", "args is null");
        return -1;
    }

    int isalive = 1;
    threadpool_t thpool = thpool_init((HAVE_SYS_CPU * 4));
    struct pollfd *pollclear = NULL;

    st_table_lock_t reqlock = {0};
    srv_socket_url_t srv;
    memset(&srv, 0, sizeof(srv_socket_url_t));
    srv.reqlock = (st_table_lock_t*)&reqlock;
    srv.obj     = tr->obj;
    net_proxy_default((sa_proxy_t*)&srv.sa2[0], (sa_socket_t*)&(tr->sa2->sac), (sa_socket_t*)&(tr->sa2->sas), NULL);

    signal(SIGPIPE, SIG_IGN);

    __MACRO_TOSCREEN__(STR_NET_DEBUG,"URL HTTP/HTTPS poll started");


#   if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
       (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))

    int i, pt;

    srv.pfd[0].fd     = srv.sa2->sac.sp;
    srv.pfd[0].events = POLLIN | POLLERR | POLLHUP;

    for (i = 1; i < __NELE(srv.pfd); i++)
    {
        srv_poll_default_(&srv, i);
    }

    while(isrun && isalive)
    {
        errno = 0;
        switch((pt = poll(srv.pfd, __NELE(srv.pfd), HAVE_NET_TIMEOUT)))
        {
            case POLL_EXPIRE:
            {
                /* timeout has expired */
#               if (defined(DEBUG_URL_INFO) && defined(DEBUG_NODAEMON))
                time_t lt = time(NULL);
                string_s tms = { ctime(&lt), 0 };
                string_trim(&tms);
                __MACRO_TOSCREEN__(STR_POLL_EX_DEBUG, "URL", tms.str);
#               endif
                break;
            }
            case POLL_ERROR:
            {
                if (errno == EINTR)
                { 
                    break;
                }
                __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "URL poll error", strerror(errno));
                isalive = 0;
                break;
            }
            default:
            {
                __POLL_EACH( continue, srv.pfd,

                    pollclear = &srv.pfd[x];

                    if (srv.pfd[x].revents == 0)
                    {
                        pollclear = NULL;
                        continue;
                    }
                    else if ((srv.pfd[x].revents & POLLHUP) || (srv.pfd[x].revents & POLLNVAL))
                    {
                        if (srv.pfd[x].fd != -1)
                        {
                            close(srv.pfd[x].fd);
                        }
                        srv.pfd[x].fd      = -1;
                        srv.pfd[x].events  = 0;
                        srv.pfd[x].revents = 0;

                        if (x == 0)
                        {
                            __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "drop bind socket, reopen to listen.. index:%d", x);
                            sa_proxy_t sa2;
                            net_sa_default(&sa2.sac);

                            errno = 0;
                            if ((isrun) && (isalive) && (net_tcp_proxy_init(&sa2, NULL, tr->bind) == 0))
                            {
                                memcpy((void*)&srv.sa2[x].sac, (void*)&sa2.sac, sizeof(sa_socket_t));
                                srv.pfd[x].fd      = sa2.sac.sp;
                                srv.pfd[x].events  = POLLIN | POLLERR | POLLHUP;
                                pollclear = NULL;
                                break;
                            }
                            __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_EV_DEBUG, "URL", '!', 
                                ((srv.pfd[x].revents & POLLHUP) ? "POLLHUP" : "POLLNVAL"),
                                0, x, pt, srv.pfd[x].fd, strerror(errno)
                            );
                            isalive = 0;
                        }
                        net_proxy_default(&srv.sa2[x], NULL, NULL, NULL);
                        pollclear = NULL;
                        break;
                    }
                    else if (srv.pfd[x].revents & POLLERR)
                    {
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_EV_DEBUG, "URL", '!', "POLLERR", -1, x, pt, srv.pfd[x].fd, strerror(errno));
                        pollclear = NULL;
                        continue;
                    }
                    else if (srv.pfd[x].revents & POLLIN)
                    {
                        __MACRO_POLLSCREEN_URL__(x, pt, srv.pfd[x].fd);

                        srv_socket_url_t *psrv = (srv_socket_url_t*)&srv;
                        psrv->spt = (sa_proxy_t*)&srv.sa2[x];

                        switch (x)
                        {
                            case 0:
                            {
                                tr->udpreg_q((void*)psrv);
                                pollclear = NULL;
                                continue;
                            }
                            default:
                            {
                                if (
                                    (psrv->spt->connect == 1) &&
                                    (
                                        (psrv->spt->sac.sp == -1) ||
                                        (psrv->spt->sas.sp == -1)
                                    )
                                   )
                                {
                                    net_close_(&srv.pfd[x]);
                                    if (psrv->spt->sac.sp > -1) { shutdown(psrv->spt->sac.sp, SHUT_RDWR); psrv->spt->sac.sp = -1; }
                                    if (psrv->spt->sas.sp > -1) { shutdown(psrv->spt->sas.sp, SHUT_RDWR); psrv->spt->sas.sp = -1; }
                                    pollclear = NULL;
                                    break;
                                }
                                else if (psrv->spt->sac.sp == srv.pfd[x].fd)
                                {
                                    if (psrv->spt->sas.sp == -1)
                                    {
                                        thpool_add_work(thpool, (net_cb)tr->udpreg_r, (void*)psrv);
                                    }
                                    else
                                    {
                                        thpool_add_work(thpool, (net_cb)tr->tcpreg_q, (void*)psrv);
                                    }
                                    pollclear = NULL;
                                }
                                else if (psrv->spt->sas.sp == srv.pfd[x].fd)
                                {
                                    thpool_add_work(thpool, (net_cb)tr->tcpreg_r, (void*)psrv);
                                    pollclear = NULL;
                                }
                            }
                        }
                    }
                );
                if (pollclear != NULL)
                {
                    net_close_(pollclear);
                    pollclear = NULL;
                }
#               if (defined(DEBUG_URL_INFO) && defined(DEBUG_NODAEMON))
                __MACRO_TOSCREEN__(STR_POLL_INF_DEBUG "%d, total queue: %zu", "URL", "threads poll wait",
                    thpool_num_threads_working(thpool), URL_NET_QUEUE
                );
#               endif
                thpool_wait(thpool);
                break;
            }
        }
    }
    __POLL_EACH(continue, srv.pfd, net_close_(&srv.pfd[x]));

// TODO: function select implement ??
//#   elif (defined(HAVE_SELECT_H) && (HAVE_SELECT_H == 1)) ||
//         (defined(HAVE_SYS_SELECT_H) && (HAVE_SYS_SELECT_H == 1))

#   else
#     error "poll/select not support for you system, header missing, see ./config.h"
#   endif

    thpool_wait(thpool);
    thpool_destroy(thpool);
    return 0;
}
