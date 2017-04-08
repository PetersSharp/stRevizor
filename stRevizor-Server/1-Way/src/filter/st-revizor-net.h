
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

#ifndef REVIZOR_NET_H
#define REVIZOR_NET_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <sys/socket.h>
#include <sys/poll.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define STR_UDPINIT_ERROR "[DNS filter]: %s failed: %s"

#define POLL_ERROR       (-1)
#define POLL_EXPIRE      (0)

#define __PTH_EXIT(A,B) \
        pthread_exit((void*)B)

typedef struct sockaddr_in sockaddr_in;
typedef struct timeval timeval;
typedef void *(*net_cb)(void *);
typedef void  (*datacopy_cb)(void*, void*);

#include "dns/st-revizor-dns.h"

typedef struct __pending_req__
{
    sockaddr_in  addr;
    uint64_t     id_ext;
    uint16_t     id_int;
    int          is_bl;
} pending_req_t;

typedef struct __sa_socket__
{
    sockaddr_in sa;
    socklen_t   sl;
} sa_socket_t;

typedef struct __srv_socket__
{
    int           sks, skc;
    sockaddr_in   sas, sac;
    size_t        pending;
    uint64_t      id_next;
    pending_req_t requests[256];
} srv_socket_t;

typedef struct __srv_transport__
{
    srv_socket_t *srv;
    void        **obj;
    net_cb        reg_q, reg_r;
    pthread_t     tid;
} srv_transport_t;

int  net_sock_unblock(int);
int  net_sa_init(sockaddr_in*, const char*, uint16_t);
void net_sa_default(sa_socket_t*);
void net_sa_clone(sa_socket_t*, sa_socket_t*);
int  net_udp_proxy_init(srv_socket_t*, string_s, string_s);
int  net_event_poll(srv_transport_t*);

#endif
