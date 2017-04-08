
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
#include "../st-revizor-hash.h"
#include "../st-revizor-net.h"
#include "../st-revizor-hash.h"
#include "st-revizor-url.h"
#include "st-revizor-filter-url-header.h"
#include "st-revizor-filter-url-response.h"
#include "st-revizor-filter-url-debug.h"
#include "st-revizor-filter-url-lang.h"
#include "../ipfw/st-revizor-ipfw-list.h"

#define __RBUF_SIZE 65536
#define PRN_SSL_TEXT "\t\t<SSL context ..>"

static hash_tree_t *htu = NULL;
static st_table_lock_t htulock = {0};
static string_s blockr = {0};

extern int isrun;
extern string_s insource[];
extern int inprocess[];

static void __url_ctors_init(void)
{
    char rbuf[30] = {0};
    time_t          now    = time(0);
    const size_t    htmlrl = (size_t)(htmlre - htmlrs);
    size_t          htmlrt = (size_t)(
        (sizeof(http_header_403) + htmlrl + 1) +
        (size_t)strftime(rbuf, 30, "%a, %d %b %Y %H:%M:%S GMT", localtime(&now))
    );
    if ((blockr.str = calloc(htmlrt, 1)) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "Allocate" STR_URL_BLOCKR, strerror(errno));
        return;
    }
    if ((blockr.sz = snprintf(blockr.str, htmlrt, http_header_403, CP rbuf, htmlrl)) <= 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "Create/Copy" STR_URL_BLOCKR, strerror(errno));
        return;
    }
    memcpy(blockr.str + blockr.sz, htmlrs, htmlrl);
    blockr.sz += htmlrl;
    blockr.str[blockr.sz] = '\0';

/*
#   if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        __MACRO_TOSCREEN__("\t--- Blocked response: [%d]\t\n[\n%s\n\t]\n", blockr.sz, blockr.str);
#   endif
*/
}
#if defined(__X_BUILD_ASM86) && defined(__GNUC__)
    void (*initurlmod)(void) ELF_SECTION_ATR(".ctors") = __url_ctors_init;
#endif

static ssize_t tcp_socket_iserror(int sidx, int iserr)
{
    switch(iserr)
    {
        case 0:
        {
            return ((sidx == 1) ? SOCK_1_RECVEND : SOCK_2_SENDEND);
        }
#       if defined(EAGAIN)
        case EAGAIN:
        {
            return ((sidx == 1) ? SOCK_1_TIMEOUT : SOCK_2_TIMEOUT);
        }
#       elif defined(EWOULDBLOCK)
        case EWOULDBLOCK:
        {
            return ((sidx == 1) ? SOCK_1_TIMEOUT : SOCK_2_TIMEOUT);
        }
#       endif
        case ETIMEDOUT:
        {
            return ((sidx == 1) ? SOCK_1_TIMEOUT : SOCK_2_TIMEOUT);
        }
        case ECONNRESET:
        {
            return ((sidx == 1) ? SOCK_1_NEGATIVE : SOCK_2_NEGATIVE);
        }
        default:
        {
            return ((sidx == 1) ? SOCK_1_SHUT : SOCK_2_SHUT);
        }
    }
}
static ssize_t tcp_recv_empty(sa_socket_t *sa)
{
    if (sa->sp < 0)
    {
        return SOCK_1_NEGATIVE;
    }

    char rbuf[2048] = {0};
    ssize_t rsz = 1, ret;

    while (0 < rsz)
    {
        errno = 0;
        rsz = recv(sa->sp, &rbuf, sizeof(rbuf), MSG_DONTWAIT);

        if (rsz == sizeof(rbuf))
        {
            continue;
        }
        else if (((rsz > 0) && (rsz < sizeof(rbuf))) ||
                 ((rsz == 0) && (errno == 0)))
        {
            return SOCK_1_RECVEND;
        }
        else if ((rsz <= 0) && (errno > 0))
        {
            if ((ret = tcp_socket_iserror(1, errno)) == SOCK_CONTINUE)
            {
                continue;
            }
            return ret;
        }
    }
    return SOCK_1_RECVEND;
}
static ssize_t tcp_recv(int sidx, sa_socket_t *sa, char *rbuf, int bsz, int flag)
{
    ssize_t rsz;

    /* TODO: timeout/nonblock  __label_set(loopr); */

    errno = 0;

    if ((rsz = recv(sa->sp, rbuf, bsz, flag)) <= 0)
    {
        if (rsz < 0)
        {
            return tcp_socket_iserror(1, errno);
        }
        return ((rsz == 0) ? ((errno == 0) ? SOCK_1_RECVEND : SOCK_1_SHUT) : SOCK_1_NEGATIVE);
    }
    return rsz;
}
static ssize_t tcp_send(int sidx, sa_socket_t *sa, char *rbuf, ssize_t rsz)
{
    if (sa->sp < 0)
    {
        return SOCK_2_NEGATIVE;
    }

    ssize_t ssz = 0;

    while (ssz < rsz)
    {
        errno = 0;
        char *pbuf = (char*)(rbuf + ssz);
        ssize_t sz = send(sa->sp, pbuf, (rsz - ssz), 0);
        if (sz < 0)
        {
            return tcp_socket_iserror(2, errno);
        }
        else if (sz == 0)
        {
            return ((errno == 0) ? SOCK_2_SENDEND : SOCK_2_SHUT);
        }
        ssz += sz;
    }
    return ssz;
}
static const char * tcp_send_error(sa_socket_t *sa, string_s *data)
{
    if (tcp_recv_empty(sa) != SOCK_1_RECVEND)
    {
        return errstrings[0];
    }
    if (tcp_send(2, sa, data->str, (ssize_t)data->sz) < 0)
    {
        return errstrings[1];
    }
    return NULL;
}
static void url_stage_prn(int is_bl, int stage, http_parse_s *hparse, char *ipstring, int ipport)
{
    if (is_bl)
    {
#       if ((defined(HAVE_HTTP_BACK_RESOLV) && (HAVE_HTTP_BACK_RESOLV == 1)) || \
            (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1)))

        __MACRO_TOSCREEN_URL__(
            "Stage %d: +block -> (reject)  -- %s:%s - %s", 'c',
            stage,
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       else
        __MACRO_TOSCREEN_URL__(
            "+block -> (reject)  -- %s:%s - %s", 'c',
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       endif
    }
    else
    {
#       if ((defined(HAVE_HTTP_BACK_RESOLV) && (HAVE_HTTP_BACK_RESOLV == 1)) || \
            (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1)))

        __MACRO_TOSCREEN_URL__(
            "Stage %d: +pass  -> (accept)  -- %s:%s - %s", 'c',
            stage,
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       else
        __MACRO_TOSCREEN_URL__(
            "+pass  -> (accept)  -- %s:%s - %s", 'c',
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       endif
    }
}
void * url_relay_a(void *obj)
{
#   if defined(DEBUG_URL_INFO)
    {
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN__("\n" STR_URL_DEBUG "url_relay_a %s", tms.str);
    }
#   endif

    __MACRO_ISSYSLOG__(const char *errstr = NULL);

    srv_socket_url_t *srv = (srv_socket_url_t*)obj;

    sa_proxy_t sap;
    net_proxy_default((sa_proxy_t*)&sap, NULL, (sa_socket_t*)&(srv->sa2[0].sas), NULL);
    sap.connect = 0;

    do {

        /* Begin client setup */
        __MACRO_ISSYSLOG__(errstr = NULL);
        __MACRO_ISSYSLOG__(errno  = 0);

        if ((sap.sac.sp = accept(srv->pfd[0].fd, (struct sockaddr*)&sap.sac.sa, &sap.sac.sl)) < 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[3]);
            break;
        }

#       if defined(HAVE_HTTP_TCPDACL) && (HAVE_HTTP_TCPDACL == 1)
        if (!acl_netwrap(&sap.sac, 0))
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[16]);
            break;
        }
#       endif

#       if defined(SO_ORIGINAL_DST)
        if (getsockopt(sap.sac.sp, SOL_IP, SO_ORIGINAL_DST, (struct sockaddr*)&sap.sad.sa, &sap.sad.sl) < 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[4]);
            break;
        }
#       else
#           error "socket options not support flag SO_ORIGINAL_DST"
#       endif
        if (net_sa_timeout(&sap.sac, 5) < 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[5]);
            break;
        }
        /* End client setup */

        net_slot_add(srv, (sa_proxy_t*)&sap, sap.sac.sp);

        return (void*)0;

    } while(0);

    net_poll_session_end(NULL, sap.sac.sp);

#   if defined(DEBUG_SYSLOG)
    if (errstr != NULL)
    {
        char ipstring[INET_ADDRSTRLEN] = {0};
        int  ipport = ntohs(sap.sac.sa.sin_port);
        inet_ntop(AF_INET, &(sap.sac.sa.sin_addr), ipstring, INET_ADDRSTRLEN);
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "url_relay_a %s:%d -> Error: %s (%s)", CP ipstring, ipport, errstr, strerror(errno));
    }
#   endif

    return (void*)-1;
}
void * url_relay_c(void *obj)
{
    int i, is_bl = 0, abs_url = 0;
    char _sdstip[INET_ADDRSTRLEN] = {0}, *sdstip = (char*)&_sdstip,
         _sdstport[18]            = {0}, *sdstport = (char*)&_sdstport;

    http_parse_s hparse;
    http_init_parse_s(&hparse);

    char rbuf[__RBUF_SIZE] = {0};
    string_s data = { CP rbuf, 0 };

    srv_socket_url_t *srv = (srv_socket_url_t*)obj;
    hash_tree_t      *htb = (hash_tree_t*)*((srv_socket_url_t*)obj)->obj;
    sa_proxy_t       *sap = (sa_proxy_t*)srv->spt;

#   if defined(DEBUG_SYSLOG)
    const char *errstr = NULL;
    int   ipport = ntohs(sap->sac.sa.sin_port);
    char _ipstring[INET_ADDRSTRLEN] = {0}, *ipstring = (char*)&_ipstring;
    inet_ntop(AF_INET, &(sap->sac.sa.sin_addr), _ipstring, INET_ADDRSTRLEN);
#   endif

#   if defined(DEBUG_URL_INFO)
    {
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN__("\n" STR_URL_DEBUG "url_relay_c %s", tms.str);
    }
#   endif

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    do {

        /* Begin client parse request */
        __MACRO_ISSYSLOG__(errstr = NULL);
        __MACRO_ISSYSLOG__(errno  = 0);

        if ((i = net_sock_iserror(sap->sac.sp)) != 0)
        {
            __MACRO_ISSYSLOG__(errstr = strerror(i));
            break;
        }
        if ((data.sz = (int)tcp_recv(1, &sap->sac, data.str, (__RBUF_SIZE - 1), MSG_PEEK)) <= 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[6]);
            break;
        }
        switch((i = http_parse_request(&data, &hparse)))
        {
            case HTTP_HEADER_EMPTY:
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[7]);
                break;
            }
            case HTTP_HEADER_NOTIMPL:
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[8]);
                break;
            }
            /* incoming CONNECT method */
#           if defined(HAVE_HTTP_ALLOW_CONNECT)
            case HTTP_PARSE_CHANGE_DST:
            {
                if (net_dns_to_sa(&sap->sad, &hparse.field[HTTP_FIELD_HOST], &hparse.field[HTTP_FIELD_PORT]) == 0)
                {
                    i = HTTP_PARSE_OK;
                    // tcp_recv_empty(&sap->sac);
                }
                break;
            }
            case HTTP_PARSE_OK:
            {
                if (net_dns_to_sa(&sap->sad, &hparse.field[HTTP_FIELD_HOST], &hparse.field[HTTP_FIELD_PORT]) < 0)
                {
                    i = HTTP_HEADER_ERROR;
                }
                break;
            }
#           endif
            /* SSL Found? */
            case HTTP_HEADER_ERROR:
            {
                if (inet_ntop(AF_INET, &(sap->sad.sa.sin_addr), _sdstip, INET_ADDRSTRLEN) == NULL)
                {
                    __MACRO_ISSYSLOG__(errstr = errstrings[9]);
                    break;
                }
                hparse.field[HTTP_FIELD_PORT].sz  = string_itoa(ntohs(sap->sad.sa.sin_port), sdstport);
                hparse.field[HTTP_FIELD_PORT].str = sdstport;
                hparse.field[HTTP_FIELD_HOST].sz  = strlen(sdstip);
                hparse.field[HTTP_FIELD_HOST].str = sdstip;
                hparse.limit                      = 2;
                abs_url                           = 1;
                i = HTTP_PARSE_SSL;
                break;
            }
            default:
            {
                break;
            }
        }
        if ((i < 0) && (i != HTTP_PARSE_SSL))
        {
            break;
        }
#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        __MACRO_TOSCREEN_URL__("client request, %d bytes:\n\t[\n\t\t%s\n\t]", 'c', data.sz,
            ((i != HTTP_PARSE_SSL) ? data.str : CP PRN_SSL_TEXT)
        );
#       else
        __MACRO_TOSCREEN_URL__("read client request: %d bytes", 'c', data.sz);
#       endif

        /* End client parse request */

        /* Begin check blocking URL HTTP/HTTPS site */

        __HASH_RLOCK(htulock,
            is_bl = (((htb) && (hash_tree_url_search(htb, (void*)&hparse, abs_url) != HASH_TREE_NOTFOUND)) ? 1 : 0)
        );
        url_stage_prn(is_bl, 1, &hparse, ipstring, ipport);

        do {
#           if ((defined(HAVE_HTTP_BACK_RESOLV) && (HAVE_HTTP_BACK_RESOLV == 1)) && \
                (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1)))

            if (is_bl) { break; }

#           elif (defined(HAVE_HTTP_BACK_RESOLV) && (HAVE_HTTP_BACK_RESOLV == 1))

            if ((is_bl) || (i != HTTP_PARSE_SSL)) { break; }

#           elif (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1))

            if ((is_bl) || (i == HTTP_PARSE_SSL)) { break; }

#           else

            break;

#           endif

            static char hbuf[1024] = {0};
            string_s hres = { CP hbuf, 0 };

            if (net_sa_to_dns((sa_socket_t*)&sap->sad, (string_s*)&hres) < 0) { break; }

            memset(data.str, '\0', __RBUF_SIZE);
            memcpy(data.str, hres.str, hres.sz);
            abs_url                           = 1;
            hparse.limit                      = 2;
            hparse.field[HTTP_FIELD_HOST].str = data.str;
            hparse.field[HTTP_FIELD_HOST].sz  = data.sz = hres.sz;
            hparse.field[HTTP_FIELD_URL].str  = NULL;

            __HASH_RLOCK(htulock,
                is_bl = (((htb) && (hash_tree_url_search(htb, (void*)&hparse, abs_url) != HASH_TREE_NOTFOUND)) ? 1 : 0)
            );
            url_stage_prn(is_bl, 2, &hparse, ipstring, ipport);
        } while(0);

        if (is_bl)
        {
            if (i == HTTP_PARSE_SSL)
            {
                break;
            }
#           if defined(DEBUG_SYSLOG)
            errstr = tcp_send_error(&sap->sac, &blockr);
#           else
            (void) tcp_send_error(&sap->sac, &blockr);
#           endif

#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("send clent 403 response: %d bytes", 'c', blockr.sz);
#           endif
            break;
        }
        /* End check blocking URL HTTP/HTTPS site */

        /* Begin remote proxy setup */
        do {
            __MACRO_ISSYSLOG__(errstr = NULL);
            __MACRO_ISSYSLOG__(errno  = 0);
            sap->connect = 0;

            if ((sap->sas.sp = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[10]);
                break;
            }
            if (net_sa_timeout(&sap->sas, 5) < 0)
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[11]);
                break;
            }
            if (connect(sap->sas.sp, (struct sockaddr *)&sap->sas.sa, sap->sas.sl) == -1)
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[12]);
                break;
            }
            sap->connect = 1;
        } while (0);

        if (!sap->connect)
        {
            data.str = CP http_header_451;
            data.sz  = sizeof(http_header_451);
            (void) tcp_send_error(&sap->sac, &data);
#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("send client 451 response: %d bytes", 'c', data.sz);
#           endif
            break;
        }

#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        __MACRO_TOSCREEN_URL__(
            "to remote proxy, fd:%d - [CONNECT %s:%u HTTP/1.0]", 'c',
            sap->sas.sp, inet_ntoa(sap->sad.sa.sin_addr), ntohs(sap->sad.sa.sin_port)
        );
#       endif

        if (dprintf(sap->sas.sp, "CONNECT %s:%u HTTP/1.0\r\n\r\n", inet_ntoa(sap->sad.sa.sin_addr), ntohs(sap->sad.sa.sin_port)) <= 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrings[13]);
            break;
        }
        if ((data.sz = (int)tcp_recv(1, &sap->sas, data.str, (__RBUF_SIZE - 1), 0)) <= 0)
        {
            if (data.sz == SOCK_1_TIMEOUT)
            {
                data.str = CP http_header_408;
                data.sz  = sizeof(http_header_408);

#               if defined(DEBUG_SYSLOG)
                if ((errstr = tcp_send_error(&sap->sac, &data)) == NULL)
#               else
                if (tcp_send_error(&sap->sac, &data) == NULL)
#               endif
                {
                    __MACRO_ISSYSLOG__(errstr = errstrings[14]);
                }
#               if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
                __MACRO_TOSCREEN_URL__("send clent 408 response: %d bytes", 'c', data.sz);
#               endif
                break;
            }
            __MACRO_ISSYSLOG__(errstr = errstrings[14]);
            break;
        }

        __MACRO_TOSCREEN_URL__("read proxy server response: %d bytes", 'c', data.sz);

#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        data.str[data.sz] = '\0';
        __MACRO_TOSCREEN_URL__("server proxy response:\n\t[\n\t\t%s\n\t]", 'c',
            ((i != HTTP_PARSE_SSL) ? data.str : CP PRN_SSL_TEXT)
        );
#       endif

        if (http_check_response(&data) != HTTP_PARSE_OK)
        {
            if (i == HTTP_PARSE_SSL)
            {
                break;
            }
#           if defined(DEBUG_SYSLOG)
            if ((errstr = tcp_send_error(&sap->sac, &data)) == NULL)
#           else
            if (tcp_send_error(&sap->sac, &data) == NULL)
#           endif
            {
                __MACRO_ISSYSLOG__(errstr = errstrings[15]);
            }
#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("send clent returnrd proxy response: %d bytes", 'c', data.sz);
#           endif
            break;
        }
        /* End remote proxy setup */

        net_tcp_buffer(sap);

        /* Commented debug socket status */
#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        net_proxy_print(sap, -100, __FILE__, __LINE__);
#       else
        __MACRO_TOSCREEN_URL__("using tcp buffer size: %d", 'c', sap->tcpbs);
#       endif

        net_slot_add(srv, sap, sap->sas.sp);

        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        __PTH_POOL_EXIT(NULL,0);

    } while(0);

    net_poll_session_end(NULL, sap->sas.sp);
    net_poll_session_end(NULL, sap->sac.sp);
    //shutdown(sap->sac.sp, SHUT_RDWR);

#   if defined(DEBUG_SYSLOG)
    if (errstr != NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "url_relay_c %s:%d -> Error: %s (%s)", ipstring, ipport, errstr, strerror(errno));
    }
#   endif

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_EXIT(NULL,-1);
}
static int url_relay_x(int bsz, sa_socket_t *s1, sa_socket_t *s2, char fid)
{

#   if defined(DEBUG_URL_PRINT)
    int   s1port                = ntohs(s1->sa.sin_port),
          s2port                = ntohs(s2->sa.sin_port);
    char _s1ip[INET_ADDRSTRLEN] = {0},
         _s2ip[INET_ADDRSTRLEN] = {0},
        *s1ip                   = (char*)&_s1ip,
        *s2ip                   = (char*)&_s2ip;
    const char *status          = NULL;
        inet_ntop(AF_INET, &(s1->sa.sin_addr), s1ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(s2->sa.sin_addr), s2ip, INET_ADDRSTRLEN);
    ssize_t ssz = 0;
#   endif

    errno = 0;

    if (s1->sp < 0)
    {
        return SOCK_1_NEGATIVE;
    }

    char rbuf[__RBUF_SIZE] = {0};
    bsz = ((bsz > __RBUF_SIZE) ? __RBUF_SIZE : bsz);
    ssize_t rsz;

    rsz = tcp_recv(1, s1, CP rbuf, bsz, 0);

#   if defined(DEBUG_URL_PRINT)
    switch(rsz)
    {
        case SOCK_1_NEGATIVE:
        {
            status = statusstrings[0];
            break;
        }
        case SOCK_1_RECVEND:
        {
            status = statusstrings[1];
            break;
        }
        case SOCK_1_SHUT:
        {
            status = statusstrings[2];
            break;
        }
        case SOCK_1_TIMEOUT:
        {
            status = statusstrings[3];
            break;
        }
        default:
        {
            status = statusstrings[4];
            break;
        }
    }
    __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\treceive [%d] bytes: [%s]",
        fid, s1ip, s1port,
        rsz, status
    );
#   endif
    if (rsz < 0)
    {
        return rsz;
    }
    else if (rsz == 0)
    {
        return SOCK_1_RECVEND;
    }
    errno = 0;

    if (s2->sp < 0)
    {
        return SOCK_2_NEGATIVE;
    }

#   if !defined(DEBUG_URL_PRINT)
    (void) tcp_send(2, s2, (char*)&(rbuf), rsz);
#   else
    switch((ssz = tcp_send(2, s2, (char*)&(rbuf), rsz)))
    {
        case SOCK_2_NEGATIVE:
        {
            status = statusstrings[5];
            break;
        }
        case SOCK_2_SENDEND:
        {
            status = statusstrings[6];
            break;
        }
        case SOCK_2_SHUT:
        {
            status = statusstrings[7];
            break;
        }
        case SOCK_2_TIMEOUT:
        {
            status = statusstrings[8];
            break;
        }
        default:
        {
            status = statusstrings[9];
            break;
        }
    }
    __MACRO_TOSCREEN__(STR_URL_DEBUG "url_relay_%c -> %s:%d\tsend    [%d] bytes: [%s]",
        fid, s2ip, s2port,
        ssz, status
    );
#   endif

    return (int)rsz;
}
static void url_relay_return(int ret, struct pollfd *pfd, sa_socket_t *s1, sa_socket_t *s2)
{
    switch(ret)
    {
       case SOCK_1_NEGATIVE:
        {
            pfd->fd      = -1;
            pfd->events  =  0;
            pfd->revents =  0;
            shutdown(s2->sp, SHUT_RDWR);
            break;
        }
       case SOCK_2_NEGATIVE:
        {
            shutdown(s1->sp, SHUT_RDWR);
            break;
        }
        /* old way */
        /*
       case SOCK_1_RECVEND:
        {
            if (flag == SOCK_ALL_SHUT)
            {
                shutdown(s1->sp, SHUT_RDWR);
                shutdown(s2->sp, SHUT_RDWR);
            }
            break;
        }
        */
       case SOCK_1_SHUT:
       case SOCK_2_SHUT:
       case SOCK_1_TIMEOUT:
       case SOCK_2_TIMEOUT:
       case SOCK_1_RECVEND:
        {
            shutdown(s1->sp, SHUT_RDWR);
            shutdown(s2->sp, SHUT_RDWR);
            break;
        }
        /*
       case SOCK_ALL_OK:
        {
            break;
        }
       case SOCK_2_SENDEND:
        {
            break;
        }
        */
       default:
        {
            break;
        }
    }
}
void * url_relay_q(void *obj)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    sa_proxy_t *spt = (sa_proxy_t*)(((srv_socket_url_t*)obj)->spt);

    url_relay_return(
        url_relay_x(spt->tcpbs, &spt->sac, &spt->sas, 'q'),
        spt->pfd,
        &spt->sac, &spt->sas
    );

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_EXIT(NULL,0);
}
void * url_relay_r(void *obj)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    sa_proxy_t *spt = (sa_proxy_t*)(((srv_socket_url_t*)obj)->spt);

    url_relay_return(
        url_relay_x(spt->tcpbs, &spt->sas, &spt->sac, 'r'),
        spt->pfd,
        &spt->sas, &spt->sac
    );

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_EXIT(NULL,0);
}

/* port list create callBack */
static void url_create_portlist(void *ipath, void *idata)
{
    if ((ipath == NULL) || (idata == NULL))
    {
        return;
    }
    string_s *path = (string_s*)ipath, *data = (string_s*)idata;

    __cleanup_new_fclose(FILE) fp = NULL;
    if ((fp = fopen(path->str, "a+")) == NULL)
    {
        return;
    }
    switch(file_size(fp))
    {
        case -1:
        {
            __cleanup_notsupport_p(auto_fcloseFILE,fp);
            return;
        }
        case 0:
        {
            break;
        }
        default:
        {
            fwrite(CP ",", 1, sizeof(char), fp);
            break;
        }
    }
    fwrite(data->str, data->sz, sizeof(char), fp);
    __cleanup_notsupport_p(auto_fcloseFILE,fp);
}
FUNEXPORT int url_init_tree(void)
{
    __BUFFER_CREATE__(bpath);
    __BUFFER_CREATE__(ppath);

    __MACRO_SNPRINTF__(bpath, -1, "%s/%s", HAVE_WATCH_BACKUP, insource[IN_FILE_BLIST].str);
    __MACRO_SNPRINTF__(ppath, -1, "%s/%s", HAVE_WATCH_BACKUP, insource[IN_FILE_PLIST].str);

    char *param[2] = { ppath.str, NULL };

    if (!file_exists_fp(&bpath))
    {
        __MACRO_TOSCREEN__(STR_URL_DEBUG "URL: Not update hash tree: %s -> source not found", bpath.str);
        return 0;
    }

    hash_tree_t *hold, *htmp = NULL;
    __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "URL: Pid -> %d. Wait, compile blacklist..", getpid());

    /* clear url port redirect rule */
#   if defined(HAVE_HTTP_CLEAR_RULE) && (HAVE_HTTP_CLEAR_RULE == 1)
    ipfw_exec(IPFW_URLDEL, (char**)param);
#   endif

    file_delete_fp(&ppath);
#   if defined(HAVE_HTTP_EMPTY_RUN) && (HAVE_HTTP_EMPTY_RUN == 1)
    htmp = hash_tree_url_read(&bpath, (void*)&ppath, (hash_portlist_cb)url_create_portlist);
#   else

    while ((htmp = hash_tree_url_read(&bpath, (void*)&ppath, (hash_portlist_cb)url_create_portlist)) == NULL)
    {
        __MACRO_TOSCREEN__(STR_URL_DEBUG "%s", "URL: Hash tree is null..");
        if (!isrun)
        {
            __MACRO_TOSCREEN__(STR_URL_DEBUG "%s", "URL: End wait hash tree source.. Return..");
            return -1;
        }
        sleep(2);
    }
#   endif

    if (!hash_tree_check(htmp))
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "URL: BlackList is empty -> Backup %s not found?", HAVE_WATCH_BACKUP);
        hash_tree_free(htmp);
#       if (!defined(HAVE_HTTP_CLEAR_RULE) || (HAVE_HTTP_CLEAR_RULE == 0))
        // TODO: save ppath or rename.. ppath
#       endif
#       if defined(HAVE_HTTP_EMPTY_RUN) && (HAVE_HTTP_EMPTY_RUN == 0)
        return 0;
#       else
        return -1;
#       endif
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "URL: BlackList -> %lu hosts. Compiled done..", htmp->eleidx[1]);

        /* add iptables dns redirect rule */
#       if defined(HAVE_HTTP_CLEAR_RULE) && (HAVE_HTTP_CLEAR_RULE == 1)
        ipfw_exec(IPFW_URLADD, (char**)param);
#       else
        if (htu == NULL)
        {
            ipfw_exec(IPFW_URLADD, (char**)param);
        }
#       endif
    }
    __HASH_WLOCK(htulock,
        hold = htu; htu = htmp
    );
    hash_tree_free(hold);
    {
        __BUILD_string_static(fpath, HAVE_WATCH_BACKUP "/" TFTP_FILE_STATURL);
        __BUFFER_CREATE__(statup);
        __MACRO_SNPRINTF__(statup, 0, "%lu|%lu|%lu", htu->eleidx[0], htu->eleidx[1], htu->eleidx[2]);
        file_create_fp((string_s*)&fpath, &statup);
        watch_set_autoconfig();
    }
    return 0;
}
static void child_SIGUSR2(int sig)
{
    if (url_init_tree() != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "URL: not update URL hash tree from signal -> %d", sig);
    }
}
FUNEXPORT int url_filter(void)
{
#   if (!defined(HAVE_HTTP_FILTER_ENABLE) || (HAVE_HTTP_FILTER_ENABLE == 0))
    __BUFFER_CREATE__(ppath);
    __MACRO_SNPRINTF__(ppath, -1, "%s/%s", HAVE_WATCH_BACKUP, insource[IN_FILE_BLIST].str);

    char *param[2] = { ppath.str, NULL };
    ipfw_exec(IPFW_URLDEL, (char**)param);

    return -1;
#   endif

#   if !defined(__X_BUILD_ASM86) || !defined(__GNUC__)
    __url_ctors_init();
#   endif
    __cleanup_set_free(char, bresfree, blockr.str);

    if ((blockr.sz == 0) || (blockr.str == NULL))
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "URL: Error init block response -> %s", strerror(errno));
        return -1;
    }
    if (url_init_tree() != 0)
    {
        __cleanup_notsupport(free, blockr.str);
        return -1;
    }
    __cleanup_set_free(hash_tree_t, htufree, htu);

    int ret = 0;
    sa_proxy_t sa2;
    string_s sup = { HAVE_HTTP_PROXY_HOST, HAVE_HTTP_PROXY_PORT },
#   if defined(HAVE_URL_BIND) && (HAVE_URL_BIND == 1)
             slo = { CP HAVE_IPFW_GW, HAVE_HTTP_CLPORT };
#   else
             slo = { NULL, HAVE_HTTP_CLPORT };
#   endif

    __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "Using HTTP/HTTPS remote proxy -> %s:%d", sup.str, sup.sz);
    __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "Using HTTP/HTTPS client listen -> %s:%d",
        ((slo.str == NULL) ? CP "0.0.0.0" : slo.str),
        slo.sz
    );

    do {

        errno = 0;
        if (net_tcp_proxy_init(&sa2, NULL, &slo) < 0)
        {
            ret = -1;
            break;
        }
        if (net_default_sa_socket_set(&sa2.sas, &sup, CP "proxy server set") < 0)
        {
            ret = -1;
            break;
        }

        srv_transport_t tr = { NULL, &sa2, (void**)&htu, &slo, (net_cb)url_relay_a, (net_cb)url_relay_c, (net_cb)url_relay_q, (net_cb)url_relay_r };
        __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "%s", "Network URL event poll -> start");

        signal(SIGUSR2, &child_SIGUSR2);

        if (url_event_poll(&tr) < 0)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "Network URL event poll -> end : %s", strerror(errno));
            ret = -1;
            break;
        }

    } while(0);

    if (ret == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "%s", "Network URL proxy init error");
    }
    __cleanup_notsupport_p(free, bresfree);
    __cleanup_notsupport_p(auto_freehash_tree_t, htufree);
    return ret;
}
