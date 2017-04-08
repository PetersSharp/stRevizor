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
#include "st-revizor-dns.h"
#include "st-revizor-filter-dns-debug.h"

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

#if (defined(DEBUG_DNS_INFO) && defined(DEBUG_NODAEMON))
static size_t net_server_queue_(srv_socket_dns_t *srv)
{
    size_t test = 0;
    __TABLE_RLOCK(srv->reqlock,
        st_get_counter(srv->reqsize, test);
    );
    return test;
}
#endif
static int net_sa_count(srv_transport_t *tr)
{
    int ret = 0;
    if (tr->sa1 != NULL)
    {
        ret = ((tr->sa1->sac.sp == -1) ? ret : (ret + 1));
        ret = ((tr->sa1->sas.sp == -1) ? ret : (ret + 1));
    }
    if (tr->sa2 != NULL)
    {
        ret = ((tr->sa2->sac.sp == -1) ? ret : (ret + 1));
        ret = ((tr->sa2->sas.sp == -1) ? ret : (ret + 1));
    }
    return ret;
}
FUNEXPORT int dns_event_poll(srv_transport_t *tr)
{
    if (
        (tr == NULL) ||
        (
            (tr->sa1 == NULL) && (tr->sa2 == NULL)
        ) ||
        (
            ((tr->udpreg_q == NULL) || (tr->udpreg_r == NULL)) &&
            ((tr->tcpreg_q == NULL) || (tr->tcpreg_r == NULL))
        )
       )
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "DNS event poll", "args is null");
        return -1;
    }

    int i, idx, isalive = 1;
    threadpool_t thpool = thpool_init(HAVE_SYS_CPU, "DNS");

    st_table_lock_t reqlock = {0};
    srv_socket_dns_t    srv;
    memset(&srv, 0, sizeof(srv_socket_dns_t));
    srv.reqlock = (st_table_lock_t*)&reqlock;
    srv.sa1   = tr->sa1;
    srv.sa2   = tr->sa2;
    srv.obj     = tr->obj;
    idx         = net_sa_count(tr);

    signal(SIGPIPE, SIG_IGN);

    __MACRO_TOSCREEN__(STR_NET_DEBUG "%d","poll socket count", idx);

#   if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
       (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))

    int pt;
    struct pollfd pfd[idx];
    memset(pfd, 0, (idx * sizeof(struct pollfd)));

    for (i = 0; i < idx; i++)
    {
        switch(i)
        {
            case 0: { pfd[i].fd = srv.sa1->sac.sp; pfd[i].events = POLLIN | POLLERR | POLLHUP; break; }
            case 1: { pfd[i].fd = srv.sa1->sas.sp; pfd[i].events = POLLIN | POLLERR | POLLHUP; break; }
            case 2: { pfd[i].fd = srv.sa2->sac.sp; pfd[i].events = POLLIN | POLLERR | POLLHUP; break; }
            case 3: { pfd[i].fd = srv.sa2->sas.sp; pfd[i].events = POLLIN | POLLERR | POLLHUP; break; }
        }
    }

    while(isrun && isalive)
    {
        errno = 0;
        switch((pt = poll(pfd, __NELE(pfd), HAVE_NET_TIMEOUT)))
        {
            case POLL_EXPIRE:
            {
                /* timeout has expired */
#               if (defined(DEBUG_DNS_INFO) && defined(DEBUG_NODAEMON))
                time_t lt = time(NULL);
                string_s tms = { ctime(&lt), 0 };
                string_trim(&tms);
                __MACRO_TOSCREEN__(STR_POLL_EX_DEBUG, "DNS", tms.str, thpool_num_threads_working(thpool), net_server_queue_(&srv));
#               endif
                break;
            }
            case POLL_ERROR:
            {
                if (errno == EINTR)
                { 
                    continue;
                }
                __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "DNS poll error", strerror(errno));
                isalive = 0;
                break;
            }
            default:
            {
                __POLL_EACH( continue, pfd,

                    if (pfd[x].revents == 0)
                    {
                        continue;
                    }
                    else if (pfd[x].revents & POLLHUP)
                    {
                        isalive = 0;
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_EV_DEBUG, "DNS", '!', "POLLHUP", idx, x, pt, pfd[x].fd, strerror(errno));
                        break;
                    }
                    else if (pfd[x].revents & POLLNVAL)
                    {
                        isalive = 0;
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_EV_DEBUG, "DNS", '!', "POLLNVAL", idx, x, pt, pfd[x].fd, strerror(errno));
                        break;
                    }
                    else if (pfd[x].revents & POLLERR)
                    {
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_EV_DEBUG, "DNS", '!', "POLLERR", idx, x, pt, pfd[x].fd, strerror(errno));
                        break;
                    }
                    else if (pfd[x].revents & POLLIN)
                    {
                        __MACRO_POLLSCREEN_DNS__(idx, x, pt, pfd[x].fd);
                        switch (x)
                        {
                            case 0:
                            {
                                if (tr->udpreg_q != NULL)
                                {
                                    thpool_add_work(thpool, (net_cb)tr->udpreg_q, (void*)&srv);
                                }
                                break;
                            }
                            case 1:
                            {
                                if (tr->udpreg_r != NULL)
                                {
                                    thpool_add_work(thpool, (net_cb)tr->udpreg_r, (void*)&srv);
                                }
                                break;
                            }
                            case 2:
                            {
                                if (tr->tcpreg_q != NULL)
                                {
                                    thpool_add_work(thpool, (net_cb)tr->tcpreg_q, (void*)&srv);
                                }
                                break;
                            }
                            case 3:
                            {
                                if (tr->tcpreg_r != NULL)
                                {
                                    thpool_add_work(thpool, (net_cb)tr->tcpreg_r, (void*)&srv);
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR "result: [%d]", "DNS poll events", " unexpected", pfd[x].revents);
                    }
                );
#               if (defined(DEBUG_DNS_INFO) && defined(DEBUG_NODAEMON))
//                __MACRO_TOSCREEN__(STR_POLL_INF_DEBUG "%d, queue used: %zu", "DNS", "threads poll wait",
//                    thpool_num_threads_working(thpool), net_server_queue_(&srv)
//                );
#               endif
                thpool_wait(thpool);
                break;
            }
        }
    }
    __POLL_EACH(continue, pfd, net_close_(&pfd[x]));

#   elif (defined(HAVE_SELECT_H) && (HAVE_SELECT_H == 1)) || \
         (defined(HAVE_SYS_SELECT_H) && (HAVE_SYS_SELECT_H == 1))

    int e[idx], r, fdmax = (tr->sa1->sac.sp < tr->sa1->sas.sp) ? tr->sa1->sas.sp : tr->sa1->sac.sp;

    for (i = 0; i < idx; i++)
    {
        switch(i)
        {
            case 0: {
                e[0] = (((tr->sa1 == NULL) || (tr->sa1->sac.sp == -1) || (tr->udpreg_q == NULL)) ? 0 : 1);
                break;
            }
            case 1: {
                e[1] = (((tr->sa1 == NULL) || (tr->sa1->sas.sp == -1) || (tr->udpreg_r == NULL)) ? 0 : 1);
                break;
            }
            case 2: {
                e[2] = (((tr->sa2 == NULL) || (tr->sa2->sac.sp == -1) || (tr->tcpreg_q == NULL)) ? 0 : 1);
                break;
            }
            case 3: {
                e[3] = (((tr->sa2 == NULL) || (tr->sa2->sas.sp == -1) || (tr->tcpreg_r == NULL)) ? 0 : 1);
                break;
            }
        }
    }
    while (isrun && isalive)
    {
        timeval cycle = {1, 0};
        fd_set fdr;
        FD_ZERO(&fdr);

        for (i = 0; i < idx; i++)
        {
            switch(i)
            {
                case 1: { if (e[0]) { FD_SET(tr->sa1->sac.sp, &fdr); } break; }
                case 0: { if (e[1]) { FD_SET(tr->sa1->sas.sp, &fdr); } break; }
                case 2: { if (e[2]) { FD_SET(tr->sa2->sac.sp, &fdr); } break; }
                case 3: { if (e[3]) { FD_SET(tr->sa2->sas.sp, &fdr); } break; }
            }
        }
        switch(select((fdmax + 1), &fdr, NULL, NULL, &cycle))
        {
            case -1:
            {
                if (errno == EINTR)
                {
                    continue;
                }
                __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "DNS select error", strerror(errno));
                isalive = 0;
                break;
            }
            case 0:
            {
                continue;
            }
            default:
            {
                if ((e[0]) && (FD_ISSET(tr->sa1->sac.sp, &fdr)))
                {
                    thpool_add_work(thpool, (net_cb)tr->udpreg_q, (void*)&srv);
                }
                if ((e[1]) && (FD_ISSET(tr->sa1->sas.sp, &fdr)))
                {
                    thpool_add_work(thpool, (net_cb)tr->udpreg_r, (void*)&srv);
                }
                if ((e[2]) && (FD_ISSET(tr->sa2->sac.sp, &fdr)))
                {
                    thpool_add_work(thpool, (net_cb)tr->tcpreg_q, (void*)&srv);
                }
                if ((e[3]) && (FD_ISSET(tr->sa2->sas.sp, &fdr)))
                {
                    thpool_add_work(thpool, (net_cb)tr->tcpreg_r, (void*)&srv);
                }
#               if (defined(DEBUG_DNS_INFO) && defined(DEBUG_NODAEMON))
                __MACRO_TOSCREEN__(STR_POLL_INF_DEBUG "%d, queue used: %zu", "DNS", "threads poll wait",
                    thpool_num_threads_working(thpool), net_server_queue_(&srv)
                );
#               endif
                thpool_wait(thpool);
                break;
            }
        }
    }
    if ((e[0]) || (e[1])) { net_close_(tr->sa1); }
    if ((e[2]) || (e[3])) { net_close_(tr->sa2); }

#   else
#     error "poll/select not support for you system, header missing, see ./config.h"
#   endif

    thpool_wait(thpool);
    thpool_destroy(thpool);
    return 0;
}
