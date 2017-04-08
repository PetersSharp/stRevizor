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

#ifndef REVIZOR_URL_H
#define REVIZOR_URL_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(URL_NET_QUEUE)
#    define URL_NET_QUEUE ((HAVE_NET_QUEUE *2) + 2)
#endif

#if defined(HAVE_BUILD_OPENMAX) && (HAVE_BUILD_OPENMAX < URL_NET_QUEUE)
#    undef URL_NET_QUEUE
#    define URL_NET_QUEUE (HAVE_BUILD_OPENMAX - 2)
#endif

#if ((!defined(HAVE_WATCH_DAEMON) || (HAVE_WATCH_DAEMON == 0)) || \
     (defined(DEBUG_SYSLOG) && (DEBUG_SYSLOG == 1)))
#     define DEBUG_URL_PRINT 1
#endif

#define __RBUF_SIZE 65536
#define __POLL_CLOSED(A,B) { __cleanup_notsupport_p(auto_freesrv_socket_set_t, (A)); return (B); }

typedef enum __url_socket_status_e {
    SOCK_SRV_SHUT     = -1,
    SOCK_SRV_NEGATIVE = -2,
    SOCK_SRV_TIMEOUT  = -3,
    SOCK_SRV_DATAEND  = -4,
    SOCK_SRV_OK       = -5,
    SOCK_CLI_SHUT     = -6,
    SOCK_CLI_NEGATIVE = -7,
    SOCK_CLI_TIMEOUT  = -8,
    SOCK_CLI_DATAEND  = -9,
    SOCK_CLI_OK       = -10,
    SOCK_ALL_SHUT     = -11,
    SOCK_IOCTLERR     = -12,
    SOCK_CONTINUE     = -13,
    SOCK_ALL_OK       =  0
} socket_status_e;

typedef enum __url_socket_select_e {
    SOCK_SERVER = 0,
    SOCK_CLIENT = 1
} socket_select_e;

typedef struct __srv_buffer_u_ {
    unsigned char    str[__RBUF_SIZE];
    ssize_t          sz;
} buffer_u;

typedef struct __srv_ipstr_s_ {
    char             ip[INET_ADDRSTRLEN];
    int              port;
} ipstr_s;

typedef struct __srv_socket_url__
{
    int             *isrun, *isalive;
    sa_proxy_t      *sap;
    void           **obj;
    st_table_lock_t *reqlock;
    buffer_u         rbuf;
    ipstr_s          ipstr;

} srv_socket_url_t;

typedef struct __srv_socket_set__
{
    char             tag;
    int              bsz, flags;
#   if defined(HAVE_SPLICE)
    int              pipe[2];
#   endif
    ssize_t          sbytes, rbytes;
    socket_select_e  sidx;
    sa_socket_t     *from, *to;
    ipstr_s         *ipstr;

} srv_socket_set_t;

    /*
    // CallBack
    // int  url_relay_a((void*)srv_socket_url_t*);
    */

int                url_poll_event(srv_transport_t*);
void               url_poll_session_end(sa_proxy_t*);
void               url_poll_session_send_error(sa_socket_t*, char*, int);
srv_socket_url_t * url_poll_session_init(srv_socket_url_t*, sa_socket_t*, sa_socket_t*, int*);

ssize_t      tcp_recv_buffer(socket_select_e, sa_socket_t*, buffer_u*, int);
ssize_t      tcp_recv(socket_select_e, sa_socket_t*, char*, int, int);
ssize_t      tcp_send(socket_select_e, sa_socket_t*, char*, ssize_t);
ssize_t      tcp_sends(socket_select_e, sa_socket_t*, string_s*);
ssize_t      tcp_transfer(srv_socket_set_t*);
ssize_t      tcp_socket_iserror(socket_select_e, int);
const char * tcp_socket_isdebug(socket_select_e, int);


FUNINLINE int url_session_timeout(time_t *tmout)
{
    do
    {
        if (*tmout == 0UL)
        {
            *tmout = (time(NULL) + (unsigned long)((HAVE_NET_TIMEOUT / 60) * 3));
            break;
        }
        if (*tmout <= time(NULL))
        {
            return 0;
        }
    } while (0);
    return 1;
}

FUNINLINE void __EV(auto_free,srv_socket_url_t)(srv_socket_url_t **x) {
    if ((x) && (*x)) {
        srv_socket_url_t *s = *x;
        if (s->sap != NULL)
        {
            free(s->sap);
        }
        free(s);
    }
}

FUNINLINE void __EV(auto_free,srv_socket_set_t)(srv_socket_set_t **x) {
#   if defined(HAVE_SPLICE)
    if ((x) && (*x)) {
        srv_socket_set_t *s = *x;
        if (s->pipe[0] > 0)
        {
            close(s->pipe[0]);
        }
        if (s->pipe[1] > 0)
        {
            close(s->pipe[1]);
        }
    }
#   endif
}

#endif

