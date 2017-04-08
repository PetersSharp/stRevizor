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
#include "../util/thpool.h"

extern int isrun;

static int net_sock_getbuf(int s, int flag)
{
    int       sbz;
    socklen_t gsz = sizeof(int);
    if (getsockopt(s, SOL_SOCKET, flag, (char*)&sbz, &gsz) < 0)
    {
        return 4096;
    }
    return sbz;
}
FUNEXPORT void net_tcp_buffer(sa_proxy_t *sap)
{
    int i, n, rbs = 4096,
        arr[]  = { sap->sac.sp, sap->sas.sp },
        flg[]  = { SO_RCVBUF, SO_SNDBUF };
    sap->tcpbs = 0;

    for (i = 0; i < __NELE(arr); i++)
    {
        if (arr[i] == -1) { continue; }
        for (n = 0; n < __NELE(flg); n++)
        {
            rbs = net_sock_getbuf(arr[i],flg[n]);
            sap->tcpbs = (((sap->tcpbs == 0) || (sap->tcpbs > rbs)) ? rbs : sap->tcpbs);
        }
    }
    sap->tcpbs = ((sap->tcpbs == 0) ? 4096 : sap->tcpbs);
}
FUNEXPORT int net_sa_timeout(sa_socket_t *sa, int tms)
{
    if (tms <= 0)
    {
        return -2;
    }
    struct timeval tmv = { tms, 0 };
    do {
#       if defined(SO_SNDTIMEO)
        if (setsockopt(sa->sp, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tmv, sizeof(struct timeval)) != 0) { break; }
#       endif
#       if defined(SO_RCVTIMEO)
        if (setsockopt(sa->sp, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tmv, sizeof(struct timeval)) != 0) { break; }
#       endif
        return 0;
    } while(0);

    __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "socket set timeout", strerror(errno));
    return -1;
}
FUNEXPORT int net_sa_to_dns(sa_socket_t *sa, string_s *dst)
{
    sa->sa.sin_family = AF_INET;
    dst->sz = 0;
    if (getnameinfo(
        (struct sockaddr*)&sa->sa,
        sa->sl,
        dst->str,
        1024,
        NULL, 0,
        NI_NUMERICSERV | NI_NAMEREQD) != 0)
    {
        return -1;
    }
    dst->sz = strlen(dst->str);
    dst->str[dst->sz] = '\0';
    return ((!dst->sz) ? -1 : 0);
}
FUNEXPORT int net_accept(int s)
{
    sa_socket_t    sax;
    net_sa_default(&sax);

    int ret = accept(s, (void*)&sax.sa, &sax.sl);
    if (ret < 0)
    {
#       if defined(EAGAIN)
        if (errno == EAGAIN)
#       elif defined(EWOULDBLOCK)
        if (errno == EWOULDBLOCK)
#       endif
        {
            return net_accept(s);
        }
        __MACRO_TOSCREEN__(STR_NET_DEBUG "[%d/%d]: %s", "Connected error", s, ret, strerror(errno));
        return ret;
    }
    __MACRO_TOSCREEN__(STR_NET_DEBUG "[%d/%d]: %s:%d", "Connected", s, ret, inet_ntoa(sax.sa.sin_addr), ntohs(sax.sa.sin_port));
    return ret;
}
FUNEXPORT int net_sock_unblock(int sock)
{
#   if defined(O_NONBLOCK)
    return fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
#   else
    return 0;
#   endif
}
FUNEXPORT int net_sock_reuseaddr(int sock)
{
    const int on = 1;
    return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}
FUNEXPORT int net_sock_iserror(int sock)
{
    int se;
    socklen_t sl = sizeof(int);

    if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &se, &sl) != 0)
    {
        return errno;
    }
    return se;
}
FUNEXPORT int net_sa_init(sockaddr_in *sa, const char *addr, uint16_t port)
{
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = htonl(INADDR_ANY); /* or INADDR_LOOPBACK for test */
    sa->sin_port = htons(port);
    if ((addr != NULL) && (inet_aton(addr, &sa->sin_addr) == 0)) { return -1; }
    return 0;
}
FUNEXPORT void net_sa_default(sa_socket_t *sa)
{
    memset(&sa->sa, 0, sizeof(sockaddr_in));
    sa->sp            = -1;
    sa->sl            = sizeof(sockaddr_in);
    sa->sa.sin_family = AF_INET;
}
FUNEXPORT void net_proxy_default(sa_proxy_t *sap, sa_socket_t *sac, sa_socket_t *sas, sa_socket_t *sad)
{
    sap->pfd   = NULL;
    sap->tcpbs = (ssize_t)4096;

    if (sac != NULL)
    {
        memcpy((void*)&sap->sac, (void*)sac, sizeof(sa_socket_t));
    }
    else
    {
        net_sa_default(&sap->sac);
    }
    if (sas != NULL)
    {
        memcpy((void*)&sap->sas, sas, sizeof(sa_socket_t));
    }
    else
    {
        net_sa_default(&sap->sas);
    }
    if (sad != NULL)
    {
        memcpy((void*)&sap->sad, sad, sizeof(sa_socket_t));
    }
    else
    {
        net_sa_default(&sap->sad);
    }
}
FUNEXPORT void net_sa_clone(sa_socket_t *sai, sa_socket_t *sao)
{
    memcpy(sao, sai, sizeof(sa_socket_t));
}
FUNEXPORT srv_transport_t * net_tr_clone(srv_transport_t *tri)
{
    srv_transport_t *tro = calloc(1, sizeof(srv_transport_t));
    memcpy(tro, tri, sizeof(srv_transport_t));
    return tro;
}
FUNEXPORT string_s * net_sa_getip(const struct sockaddr *sa, string_s *ret)
{
    switch(sa->sa_family)
    {
        case AF_INET:
        {
            inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), ret->str, INET_ADDRSTRLEN);
            break;
        }
        case AF_INET6:
        {
            inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), ret->str, INET6_ADDRSTRLEN);
            break;
        }
        default:
        {
            memcpy(ret->str, CP "IP family unknown", 17);
            ret->str[ret->sz] = '\0';
        }
    }
    if (ret->str == NULL)
    {
        if (errno != 0)
        {
            char *err = strerror(errno);
            memcpy(ret->str, err, strlen(err));
            ret->sz  = errno;
        }
        else
        {
            memcpy(ret->str, CP "IP convert unknown error, bad or fake IP address?", 49);
            ret->str[ret->sz] = '\0';
        }
    }
    return ret;
}
FUNEXPORT int net_dns_to_sa(sa_socket_t *sa, string_s *host, string_s *port)
{
    int i, ret = 0;
    struct addrinfo hints = {0}, *sinfo = NULL, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((i = getaddrinfo(host->str, NULL, &hints, &sinfo)) != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_NET_ERROR "-> Net, get DNS address: %s:%s -> %s",
            host->str, port->str,
            gai_strerror(i)
        );
        ret = -1;
    }
    else
    {
        errno = 0;

        for(p = sinfo; p != NULL; p = p->ai_next)
        {
            if (p->ai_family == AF_INET)
            {
                net_sa_default(sa);
                struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
                sa->sa.sin_family = AF_INET;
                sa->sa.sin_port   = htons(atoi(port->str));
                sa->sa.sin_addr.s_addr = ipv4->sin_addr.s_addr;
                break;
            }
        }
    }
    if (sinfo != NULL)
    {
        freeaddrinfo(sinfo);
    }
    return ret;
}
static int net_all_proxy_client_init(int type, sa_proxy_t *sap, string_s *host)
{
    memset(&sap->sac, 0, sizeof(sa_socket_t));
    sap->sac.sl = sizeof(sap->sac.sa);

    if ((sap->sac.sp = socket(AF_INET, type, 0)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "socket()", strerror(errno));
        return -1;
    }
    if (type == SOCK_STREAM)
    {
        if (net_sock_reuseaddr(sap->sac.sp) < 0)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "tcp socket option (REUSEADDR)", strerror(errno));
            return -1;
        }
    }
    if (net_sock_unblock(sap->sac.sp) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "unblock()", strerror(errno));
        return -1;
    }
    if (net_sa_init(&sap->sac.sa, host->str, host->sz) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "socket init (cli)", strerror(errno));
        return -1;
    }
    if (bind(sap->sac.sp, (struct sockaddr*)&sap->sac.sa, sap->sac.sl) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "bind()", strerror(errno));
        return -1;
    }
    if (type == SOCK_STREAM)
    {
        if (listen(sap->sac.sp, HAVE_NET_QUEUE) < 0)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "tcp listen", strerror(errno));
            return -1;
        }
    }
    /* end client socket */
    sap->sas.sp = -1;
    return 0;
}
static int net_all_proxy_server_init(int type, sa_proxy_t *sap, string_s *host)
{
    memset(&sap->sas, 0, sizeof(sa_socket_t));
    sap->sas.sl = sizeof(sap->sas.sa);

    errno = 0;
    if ((sap->sas.sp = socket(AF_INET, type, 0)) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "socket()", strerror(errno));
        return -1;
    }
    errno = 0;
    if (net_sock_unblock(sap->sas.sp) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "unblock()", strerror(errno));
        return -1;
    }
    errno = 0;
    if (net_sa_init(&sap->sas.sa, host->str, host->sz) < 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "socket init (srv)", strerror(errno));
        return -1;
    }
    errno = 0;
    if (
        (connect(sap->sas.sp, (void*)&sap->sas.sa, sap->sas.sl) < 0) &&
        (errno != EINPROGRESS)
       )
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, "connect()", strerror(errno));
        return -1;
    }
    /* end server socket */
    return 0;
}
static int net_proxy_init_(int type, sa_proxy_t *sap, string_s *upset, string_s *localset)
{
    if (localset != NULL)
    {
        if (net_all_proxy_client_init(type, sap, localset) == -1) { return -1; }
    }
    if (upset != NULL)
    {
        return net_all_proxy_server_init(type, sap, upset);
    }
    return (((localset == NULL) && (upset == NULL)) ? -1 : 0);
}
FUNEXPORT void net_default_proxy_init(sa_proxy_t *sap)
{
    memset(sap, 0, sizeof(sa_proxy_t));
    sap->sac.sa.sin_family = sap->sas.sa.sin_family = AF_INET;
    sap->sac.sl = sap->sas.sl = sizeof(sap->sac.sa);
    sap->sac.sp = sap->sas.sp = -1;
}
FUNEXPORT int net_default_sa_socket_set(sa_socket_t *sa, string_s *upset, char *strtype)
{
    if (upset == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, strtype, "empty server address string");
        return -1;
    }
    net_sa_default(sa);
    sa->sa.sin_family = AF_INET;
    sa->sa.sin_port   = htons(upset->sz);
    if (inet_pton(AF_INET, upset->str, &sa->sa.sin_addr) <= 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_NETINIT_ERROR, strtype, strerror(errno));
        return -1;
    }
    return 0;
}
FUNEXPORT int net_tcp_proxy_init(sa_proxy_t *sap, string_s *upset, string_s *localset)
{
    int s = net_proxy_init_(SOCK_STREAM, sap, upset, localset);
    if (s > 0)
    {
        do
        {
            const int one = 1;
#           if defined(SO_NOSIGPIPE)
            if (setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one)) < 0)
            {
                break;
            }
#           endif
#           if defined(TCP_NODELAY)
            if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) < 0)
            {
                break;
            }
#           endif

            return s;

        } while(0);

        close(s);
        return -1;
    }
    return s;
}
FUNEXPORT int net_udp_proxy_init(sa_proxy_t *sap, string_s *upset, string_s *localset)
{
    return net_proxy_init_(SOCK_DGRAM, sap, upset, localset);
}
