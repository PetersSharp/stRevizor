
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#if defined(HAVE_SCHED_H)
#include <sched.h>
#endif

#if defined(HAVE_IFADDRS_H)
#include <ifaddrs.h>
#endif

#include "st-revizor-lock.h"
#include "../st-revizor-inline.h"

#if   (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1))
#     include <poll.h>
#elif (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
#     include <sys/poll.h>
#elif (defined(HAVE_SELECT_H) && (HAVE_SELECT_H == 1))
#     include <select.h>
#elif (defined(HAVE_SYS_SELECT_H) && (HAVE_SYS_SELECT_H == 1))
#     include <sys/select.h>
#endif

#if !defined(HAVE_BUILD_OPENMAX)
#    define HAVE_BUILD_OPENMAX 1024
#endif

#if !defined(HAVE_NET_QUEUE)
#    define HAVE_NET_QUEUE 1024
#endif

#if !defined(HAVE_NET_TIMEOUT)
#    define HAVE_NET_TIMEOUT 3500
#endif

#if !defined(SOL_TCP)
#    define SOL_TCP 6
#endif

#if !defined(TCP_USER_TIMEOUT)
#   define TCP_USER_TIMEOUT 18
#endif

#define POLL_ERROR       (-1)
#define POLL_EXPIRE      (0)

#define __PTH_EXIT(A,B) \
    { if (A != NULL) { __SYNC_MEM(free(A)); } \
            pthread_exit((void*)B); }

#define __PTH_POOL_EXIT(A,B) \
    { if (A != NULL) { __SYNC_MEM(free(A)); } \
            return (void*)B; }

#define __PTH_POOL_TCP_EXIT(A,B) \
    { if (A != -1) { close(A); } \
            return (void*)B; }

#define __POLL_EACH(A,B,C) \
    { int x; errno = 0; \
        __SYNC_MEM( \
        for (x = 0; x < __NELE(B); x++) { \
            if (B[x].fd == -1) { A; } C; }); }

typedef struct sockaddr_in sockaddr_in;
typedef struct timeval timeval;
typedef void *(*net_cb)(void *);
typedef void  (*datacopy_cb)(void*, void*);

typedef struct __pending_req__
{
    sockaddr_in  addr;
    uint64_t     id_ext;
    uint16_t     id_int;
    int          is_bl;
} pending_req_t;

typedef struct __sa_socket__
{
    int         sp;
    sockaddr_in sa;
    socklen_t   sl;
} sa_socket_t;

typedef struct __sa_proxy__
{
    sa_socket_t    sas, sac, sad;
#   if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
       (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
    struct pollfd *pfd;
#   endif
    int            tcpbs, connect;
} sa_proxy_t;

typedef struct __srv_transport__
{
    sa_proxy_t   *sa1;
    sa_proxy_t   *sa2;
    void        **obj;
    string_s     *bind;
    net_cb        udpreg_q, udpreg_r, tcpreg_q, tcpreg_r;
} srv_transport_t;

void net_tcp_buffer(sa_proxy_t*);
int  net_accept(int);
int  net_sock_iserror(int);
int  net_sock_unblock(int);
int  net_sock_reuseaddr(int);
int  net_sa_init(sockaddr_in*, const char*, uint16_t);
void net_sa_default(sa_socket_t*);
void net_sa_clone(sa_socket_t*, sa_socket_t*);
int  net_sa_timeout(sa_socket_t*, int);
int  net_sa_to_dns(sa_socket_t*, string_s*);
int  net_dns_to_sa(sa_socket_t*, string_s*, string_s*);

void net_proxy_default(sa_proxy_t*, sa_socket_t*, sa_socket_t*, sa_socket_t*);

string_s *net_sa_getip(const struct sockaddr*, string_s*);

void net_default_proxy_init(sa_proxy_t*);
int  net_default_sa_socket_set(sa_socket_t*, string_s*, char*);
int  net_udp_proxy_init(sa_proxy_t*, string_s*, string_s*);
int  net_tcp_proxy_init(sa_proxy_t*, string_s*, string_s*);

int acl_netwrap(sa_socket_t*, int);

#if !defined(HAVE_WATCH_DAEMON) || (HAVE_WATCH_DAEMON != 1)
void net_proxy_print(sa_proxy_t*, int, char*, int);
void net_sa_print(sa_socket_t*, char*, char*, int);

#    if (defined(HAVE_POLL_H) && (HAVE_POLL_H == 1)) || \
        (defined(HAVE_SYS_POLL_H) && (HAVE_SYS_POLL_H == 1))
void net_poll_print(struct pollfd*, char*, int);
#    else
#        define net_poll_print(...)
#    endif

#else

#define net_proxy_print(...)
#define net_sa_print(...)
#define net_poll_print(...)

#endif

FUNINLINE int net_idset(volatile int *lock)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    if ((int)(__sync_fetch_and_add(lock, 0)) > (INT_MAX - 1))
    {
        __sync_fetch_and_and(lock, 0);
    }
    else
    {
        __sync_add_and_fetch(lock, 1);
    }
    return __sync_fetch_and_add(lock, 0);
#   else
    int ret;
    __SYNC_MEM(ret = *lock); ret++;
    if (ret > (INT_MAX - 1))
    {
        ret = 0;
    }
    __SYNC_MEM(memcpy((void*)lock, (void*)&ret, sizeof(int)));
    return ret;
#   endif
}
FUNINLINE int net_idget(volatile int *lock)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    return __sync_fetch_and_add(lock, 0);
#   else
    int ret;
    __SYNC_MEM(ret = *lock);
    return ret;
#   endif
}

#endif
