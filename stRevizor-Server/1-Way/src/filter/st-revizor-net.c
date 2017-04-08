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

#include <pthread.h>
#include <signal.h>

#include "st-revizor-net.h"

extern int isrun;

int net_sock_unblock(int sock)
{
    int ret = fcntl(sock, F_GETFL);
    return (ret < 0) ? ret : fcntl(sock, F_SETFL, ret | O_NONBLOCK);
}
int net_sa_init(sockaddr_in *sa, const char *addr, uint16_t port)
{
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = 0;
    sa->sin_port = htons(port);

    if (addr && ! inet_aton(addr, &sa->sin_addr)) { return -1; }
    return 0;
}
void net_sa_default(sa_socket_t *sa)
{
    sa->sl = sizeof(sockaddr_in);
    memset(&sa->sa, 0, sa->sl);
}
void net_sa_clone(sa_socket_t *sai, sa_socket_t *sao)
{
    memcpy(sao, sai, sizeof(sa_socket_t));
}
srv_transport_t * net_tr_clone(srv_transport_t *tri)
{
    srv_transport_t *tro = calloc(1, sizeof(srv_transport_t));
    memcpy(tro, tri, sizeof(srv_transport_t));
    return tro;
}
int net_udp_proxy_init(srv_socket_t *srv, string_s upset, string_s localset)
{
    if ((srv->skc = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "socket()", strerror(errno));
        return -1;
    }
    if (net_sock_unblock(srv->skc) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "unblock()", strerror(errno));
        return -1;
    }
    if (net_sa_init(&srv->sac, localset.str, localset.sz) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "socket init (cli)", strerror(errno));
        return -1;
    }
    if (bind(srv->skc, (void*)&srv->sac, sizeof(srv->sac)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "bind()", strerror(errno));
        return -1;
    }
    // end client socket

    if ((srv->sks = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "socket()", strerror(errno));
        return -1;
    }
    if (net_sock_unblock(srv->sks) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "unblock()", strerror(errno));
        return -1;
    }
    if (net_sa_init(&srv->sas, upset.str, upset.sz) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "socket init (srv)", strerror(errno));
        return -1;
    }
    if (connect(srv->sks, (void*)&srv->sas, sizeof(srv->sas)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "connect()", strerror(errno));
        return -1;
    }
    // end server socket
    return 0;
}
int net_event_poll(srv_transport_t *tr)
{
    if (
        (tr == NULL) ||
        (tr->srv == NULL) ||
        (tr->reg_q == NULL) ||
        (tr->reg_r == NULL)
       )
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "event poll", "args is null");
        return -1;
    }

    struct pollfd pfd[] = {
        { .fd = tr->srv->sks, .events = POLLIN },
        { .fd = tr->srv->skc, .events = POLLIN }
    };

    signal(SIGPIPE, SIG_IGN);

    int i;
    while(isrun)
    {
        errno = 0;

        switch(poll(pfd, 2, HAVE_DNS_TIMEOUT))
        {
            case POLL_EXPIRE:
            {
                /* timeout has expired */
                break;
            }
            case POLL_ERROR:
            {
                if (errno == EINTR)
                { 
                    continue;
                }
                __MACRO_TOSYSLOG__(LOG_ERR, STR_UDPINIT_ERROR, "poll error", "label is POLL_ERROR");
                return -1;
            }
            default:
            {
                errno = 0;
                for (i = 0; i < __NELE(pfd); i++)
                {
                    if (pfd[i].revents & POLLIN)
                    {
                        pthread_t        tid;
                        srv_transport_t  tro;
                        memcpy(&tro, tr, sizeof(srv_transport_t));

                        if (i == 1)
                        {
                            if (pthread_create(&tid, NULL, (net_cb)tr->reg_q, (void*)&tro) != 0) { break; }
                        } else {
                            if (pthread_create(&tid, NULL, (net_cb)tr->reg_r, (void*)&tro) != 0) { break; }
                        }
                        pthread_join(tid, NULL);
                    }
                }
            }
        }
    }
    return 0;
}
