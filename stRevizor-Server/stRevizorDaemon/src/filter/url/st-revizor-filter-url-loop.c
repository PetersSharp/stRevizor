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

#define __URL_ERR_LOOP 1
#if defined(REVIZOR_URL_LANG_H)
#   undef REVIZOR_URL_LANG_H
#endif
#include "st-revizor-filter-url-lang.h"

#define __URL_ERR_LOOP 1
#if defined(REVIZOR_URL_RESPONSE_H)
#   undef REVIZOR_URL_RESPONSE_H
#endif
#include "st-revizor-filter-url-response.h"

#include "../../util/thpool.h"

#if (!defined(HAVE_SYS_CPU) || !defined(HAVE_HTTP_NUMTH))
#   error "no poll thread number defined: HAVE_SYS_CPU or HAVE_HTTP_NUMTH, check config.h or run ./configure"
#endif

#if defined(URL_NUM_THREAD)
#   undef URL_NUM_THREAD
#endif

#define URL_NUM_THREAD (int)((int)(HAVE_HTTP_NUMTH / HAVE_SYS_CPU) * (int)HAVE_SYS_CPU)

extern int isrun;

FUNEXPORT void url_poll_session_end(sa_proxy_t *sap)
{
    if (sap != NULL)
    {
        if (sap->sac.sp != -1)
        {
            shutdown(sap->sac.sp, SHUT_RDWR);
            close(sap->sac.sp);
        }
        if (sap->sas.sp != -1)
        {
            shutdown(sap->sas.sp, SHUT_RDWR);
            close(sap->sas.sp);
        }
    }
}

FUNEXPORT srv_socket_url_t * url_poll_session_init(srv_socket_url_t *srv, sa_socket_t *sac, sa_socket_t *sas, int *err)
{
    srv_socket_url_t *csrv = NULL;
    do
    {
        if ((csrv = calloc(1, sizeof(srv_socket_url_t))) == NULL)
        {
            break;
        }
        memcpy((void*)csrv, (void*)srv, sizeof(srv_socket_url_t));

        if ((csrv->sap = calloc(1, sizeof(sa_proxy_t))) == NULL)
        {
            break;
        }
        if (net_sock_unblock(sac->sp) == -1)
        {
            break;
        }
        if (net_sa_timeout(sac, (int)(HAVE_NET_TIMEOUT / 10 / 60)) < 0)
        {
            break;
        }
        if (tcp_recv_buffer(SOCK_CLIENT, sac, &csrv->rbuf, 0) <= 0)
        {
            break;
        }

        net_proxy_default(csrv->sap, sac, sas, NULL);

#       if (defined(DEBUG_NODAEMON) && (defined(DEBUG_URL_INFO) || defined(DEBUG_URL_SENDRCV) || defined(DEBUG_SYSLOG)))
        csrv->ipstr.port = ntohs(csrv->sap->sac.sa.sin_port);
        inet_ntop(AF_INET, &(csrv->sap->sac.sa.sin_addr), csrv->ipstr.ip, INET_ADDRSTRLEN);
#       endif

        *err = 0;
        return csrv;

    } while(0);

    *err = errno;
    auto_freesrv_socket_url_t((srv_socket_url_t**)&csrv);
    return NULL;
}

FUNEXPORT void url_poll_session_send_error(sa_socket_t *sac, char *errtag, int esz)
{
    string_s data = { errtag, esz };
    (void) tcp_send_error(sac, data);
    shutdown(sac->sp, SHUT_RDWR);
    close(sac->sp);
}

FUNEXPORT int url_poll_event(srv_transport_t *tr)
{
    if ((tr == NULL) || (tr->sa2 == NULL))
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "URL event poll", "args is null");
        return -1;
    }

    int pt, err, isalive = 1;
    threadpool_t     thpool  = thpool_init(URL_NUM_THREAD,"URL");

    st_table_lock_t  reqlock = {0};
    srv_socket_url_t srv;

    memset(&srv, 0, sizeof(srv_socket_url_t));
    srv.reqlock = (st_table_lock_t*)&reqlock;
    srv.obj     = tr->obj;
    srv.isrun   = &isrun;
    srv.isalive = &isalive;

    struct pollfd pfd[] =
    {
        { .fd = tr->sa2->sac.sp, .events = POLLIN | POLLERR | POLLHUP | POLLNVAL }
    };

    signal(SIGPIPE, SIG_IGN);

    __MACRO_TOSCREEN__(STR_NET_DEBUG,"URL HTTP/HTTPS poll started");

    while(isrun && isalive)
    {
        errno = 0;
        switch((pt = poll(pfd, __NELE(pfd), HAVE_NET_TIMEOUT)))
        {
            case POLL_EXPIRE:
            {
                /* timeout has expired */
#               if (defined(DEBUG_URL_INFO) && defined(DEBUG_NODAEMON))
                time_t lt = time(NULL);
                string_s tms = { ctime(&lt), 0 };
                string_trim(&tms);
                __MACRO_TOSCREEN__(STR_POLL_EX_DEBUG, "URL", tms.str, thpool_num_threads_working(thpool), URL_NUM_THREAD);
#               endif
                break;
            }
            case POLL_ERROR:
            {
                if ((errno == EINTR) || (errno == EAGAIN))
                { 
                    break;
                }
                __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "URL poll error", strerror(errno));
                isalive = 0;
                break;
            }
            default:
            {
                if (pfd[0].revents == 0)
                {
                    continue;
                }
                else if ((pfd[0].revents & POLLHUP) || (pfd[0].revents & POLLNVAL))
                {
                    __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "drop bind socket, reopen to listen.. port:%d", pfd[0].fd);

                    if (pfd[0].fd != -1)
                    {
                        close(pfd[0].fd);
                    }

                    sa_proxy_t sa2;
                    net_sa_default(&sa2.sac);

                    errno = 0;
                    if ((isrun) && (isalive) && (net_tcp_proxy_init(&sa2, NULL, tr->bind) == 0))
                    {
                        memcpy((void*)&tr->sa2->sac, (void*)&sa2.sac, sizeof(sa_socket_t));
                        pfd[0].fd      = sa2.sac.sp;
                        pfd[0].events  = POLLIN | POLLERR | POLLHUP;
                        break;
                    }
                    __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_URL_DEBUG, '!', 
                        ((pfd[0].revents & POLLHUP) ? "POLLHUP" : "POLLNVAL"),
                        pt, pfd[0].fd, strerror(errno)
                    );
                    isalive        = 0;
                    pfd[0].fd      = -1;
                    pfd[0].events  = 0;
                    pfd[0].revents = 0;
                    break;
                }
                else if (pfd[0].revents & POLLERR)
                {
                    __MACRO_TOSYSLOG__(LOG_ERR, STR_POLL_URL_DEBUG, '!', "POLLERR", pt, pfd[0].fd, strerror(errno));
                    continue;
                }
                else if (pfd[0].revents & POLLIN)
                {
                    __MACRO_POLLSCREEN_URL__(0, pt, pfd[0].fd);
                    srv_socket_url_t *csrv;
                    sa_socket_t sac;
                    net_sa_default(&sac);

                    if ((sac.sp = accept(pfd[0].fd, (struct sockaddr*)&sac.sa, &sac.sl)) < 0)
                    {
#                       if defined(EAGAIN)
                        if ((errno != EAGAIN) && (errno != EINTR))
#                       elif defined(EWOULDBLOCK)
                        if ((errno != EWOULDBLOCK) && (errno != EINTR))
#                       endif
                        {
                            __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "URL poll accept error: %s", strerror(errno));
                        }
                        continue;
                    }
                    if (thpool_num_threads_working(thpool) == (int)(URL_NUM_THREAD - 1))
                    {
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "URL poll is full [%d/%d], abort connection [%d]!",
                            thpool_num_threads_working(thpool), (int)URL_NUM_THREAD, sac.sp
                        );
                        url_poll_session_send_error(&sac, CP http_header_500pf, __CSZ(http_header_500pf));
                        break;
                    }
                    if ((csrv = url_poll_session_init(
                            (srv_socket_url_t*)&srv,
                            (sa_socket_t*)&sac,
                            (sa_socket_t*)&(tr->sa2->sas),
                            &err
                        )) == NULL)
                    {
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "client init error -> port %d/%d : %s",
                            pfd[0].fd, sac.sp,
                            ((err > 0) ? strerror(errno) : CP "-")
                        );
                        url_poll_session_send_error(&sac, CP http_header_500me, __CSZ(http_header_500me));
                        break;
                    }
                    thpool_add_work(thpool, (net_cb)tr->udpreg_q, (void*)csrv);
                }
                break;
            }
        }
#       if (defined(DEBUG_URL_INFO) && defined(DEBUG_NODAEMON))
//        __MACRO_TOSCREEN__(STR_POLL_INF_STAT "%d, total queue: %d", "URL", "threads poll wait",
//            thpool_num_threads_working(thpool), URL_NUM_THREAD
//        );
#       endif
    }

    thpool_wait(thpool);
    thpool_destroy(thpool);
    return 0;
}
