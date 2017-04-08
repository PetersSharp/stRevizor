
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
#include "st-revizor-filter-url-debug.h"
#include "st-revizor-url.h"
#include "st-revizor-filter-url-tcp.h"
#include "st-revizor-filter-url-header.h"
#include "../ipfw/st-revizor-ipfw-list.h"

#define __URL_ERR_ENGINE 1
#if defined(REVIZOR_URL_LANG_H)
#   undef REVIZOR_URL_LANG_H
#endif
#include "st-revizor-filter-url-lang.h"

#define __URL_ERR_ENGINE 1
#if defined(REVIZOR_URL_RESPONSE_H)
#   undef REVIZOR_URL_RESPONSE_H
#endif
#include "st-revizor-filter-url-response.h"

static hash_tree_t *htu = NULL;
static st_table_lock_t htulock = {0};
static string_s blockr = {0};
static unsigned long locip[50] = {0};

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

#   if defined(HAVE_IFADDRS_H)
    int idx = 0;
    struct ifaddrs *ifap = NULL, *ifa;
    struct sockaddr_in *sa;

    if (getifaddrs(&ifap) == 0)
    {
        for (ifa = ifap; ((ifa) && (idx < __NELE(locip))); ifa = ifa->ifa_next)
        {
            if ((ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_INET))
            {
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                locip[idx++] = sa->sin_addr.s_addr;
            }
        }
    }
    freeifaddrs(ifap);
#   endif
}
#if defined(__X_BUILD_ASM86) && defined(__GNUC__)
    void (*initurlmod)(void) ELF_SECTION_ATR(".ctors") = __url_ctors_init;
#endif

static int url_check_dst(sa_socket_t *sa, http_parse_status_e hstat)
{
    if ((unsigned long)sa->sa.sin_addr.s_addr == 0UL)
    {
        return ((hstat == HTTP_PARSE_SSL) ? 1 : 0);
    }
    for (int i = 0; i < __NELE(locip); i++)
    {
        if (locip[i] == 0UL)
        {
            return 0;
        }
        if (
            (locip[i] == (unsigned long)sa->sa.sin_addr.s_addr) &&
            (ntohs(sa->sa.sin_port) == (int)HAVE_HTTP_CLPORT)
           )
        {
            return 1;
        }
    }
    return 0;
}

static void url_stage_prn(int is_bl, int stage, srv_socket_url_t *srv, http_parse_s *hparse)
{
    if (is_bl)
    {
#       if ((defined(HAVE_HTTP_BACK_RESOLV) && (HAVE_HTTP_BACK_RESOLV == 1)) || \
            (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1)))

        __MACRO_TOSCREEN_URL__(
            "", "stage %d: +block -> (reject)  -- %s:%s - %s", 'f',
            stage,
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       else
        __MACRO_TOSCREEN_URL__(
            "", "+block -> (reject)  -- %s:%s - %s", 'f',
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
            "", "stage %d: +pass  -> (accept)  -- %s:%s - %s", 'f',
            stage,
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       else
        __MACRO_TOSCREEN_URL__(
            "", "+pass  -> (accept)  -- %s:%s - %s", 'f',
            hparse->field[HTTP_FIELD_HOST].str,
            hparse->field[HTTP_FIELD_PORT].str,
            ((hparse->field[HTTP_FIELD_URL].str == NULL) ? CP "/" : hparse->field[HTTP_FIELD_URL].str)
        );
#       endif
    }
}
static int url_relay_f(srv_socket_url_t *srv)
{
    int i,
        is_bl = 0,
        abs_url = 0;
    http_parse_s hparse;
    http_init_parse_s(&hparse);

    char rbuf[__RBUF_SIZE] = {0};
    string_s data = { CP rbuf, (int)srv->rbuf.sz };
    memcpy((void*)&rbuf, (void*)srv->rbuf.str, (int)srv->rbuf.sz);

    hash_tree_t      *htb = (hash_tree_t*)*srv->obj;
    sa_proxy_t       *sap = (sa_proxy_t*)srv->sap;

    __MACRO_ISSYSLOG__(const char *errstr = NULL);

printf("+++\turl_relay_f (1) %s\n", strerror(errno));

    do {

        /* Begin client parse request */
        __MACRO_ISSYSLOG__(errno  = 0);

        if ((i = net_sock_iserror(sap->sac.sp)) != 0)
        {
            __MACRO_ISSYSLOG__(errstr = strerror(i));
            break;
        }
printf("+++\turl_relay_f (2) %s\n", strerror(errno));

#       if defined(SO_ORIGINAL_DST)
        if (getsockopt(sap->sac.sp, SOL_IP, SO_ORIGINAL_DST, (struct sockaddr*)&sap->sad.sa, &sap->sad.sl) < 0)
        {
            __MACRO_ISSYSLOG__(errstr = errstrengine[4]);
            break;
        }
#       else
#           warning "socket options not support flag SO_ORIGINAL_DST, SSL client NOT compatible, run ./configure and disable URL filter"
#       endif

printf("+++\turl_relay_f (2:0) %s\n", strerror(errno));

        switch((i = http_parse_request(&data, &hparse)))
        {
            case HTTP_HEADER_EMPTY:
            {
                __MACRO_ISSYSLOG__(errstr = errstrengine[7]);
                break;
            }
            case HTTP_HEADER_NOTIMPL:
            {
                __MACRO_ISSYSLOG__(errstr = errstrengine[8]);
                break;
            }
            /* incoming CONNECT method */
#           if defined(HAVE_HTTP_ALLOW_CONNECT)
            case HTTP_PARSE_CHANGE_DST:
            {
                if (net_dns_to_sa(&sap->sad, &hparse.field[HTTP_FIELD_HOST], &hparse.field[HTTP_FIELD_PORT]) == 0)
                {
                    i = HTTP_PARSE_OK;
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

printf("+++\turl_relay_f (HTTP_HEADER_ERROR) %s\n", strerror(errno));

                /* Debug SNI */
#               if 0
                {
                    FILE *fp = fopen("/home/admin/SNI-SSL.dump", "a+");
                    fprintf(fp, "\nClient: %s:%d\n", CP srv->ipstr.ip, srv->ipstr.port);
                    fwrite(srv->rbuf.str, srv->rbuf.sz, sizeof(unsigned char), fp);
                    fclose(fp);
                }
#               endif

                /* Parse TLS-SNI */
#               if defined(HAVE_HTTPS_SNI_RESOLV)
                if (https_parse_sni(&data, &hparse) == HTTP_PARSE_OK)
                {
printf("+++\turl_relay_f (https_parse_sni OK) %s\n", strerror(errno));

                    __MACRO_TOSCREEN_URL__("","TLS SNI header found: %s:%s", 'f',
                        hparse.field[HTTP_FIELD_HOST].str,
                        hparse.field[HTTP_FIELD_PORT].str
                    );
printf("+++\turl_relay_f (https_parse_sni net_dns_to_sa 1) %s\n", strerror(errno));

                    if (net_dns_to_sa(&sap->sad, &hparse.field[HTTP_FIELD_HOST], &hparse.field[HTTP_FIELD_PORT]) < 0)
                    {
                        break;
                    }
printf("+++\turl_relay_f (https_parse_sni net_dns_to_sa 2) %s\n", strerror(errno));

                    abs_url = 1;
                    i = HTTP_PARSE_SSL;
                    break;
                }
#               endif

printf("+++\turl_relay_f (https_parse_sni END) %s\n", strerror(errno));

#               if defined(SO_ORIGINAL_DST)
                int err = 0;
                data.sz = __RBUF_SIZE;

                /* Parse destenation, if SNI not found or disable.
                    Using iptables SNAT transparent mode - found loop error, broken dst ip address.
                    TODO: add iptables TPROXY implementation support.
                 */
                if (https_parse_dst(&sap->sad, &data, &hparse, &err) == HTTP_PARSE_OK)
                {
                    abs_url = 1;
                    i = HTTP_PARSE_SSL;
                    break;
                }
#               endif

printf("+++\turl_relay_f (https_parse_dst END) %s\n", strerror(errno));

                errno = 0;
                __MACRO_ISSYSLOG__(if (err > 0) { errstr = errstrengine[err]; });
                break;
            }
            default:
            {
                break;
            }
        }

printf("+++\turl_relay_f (3:0) %s\n", strerror(errno));

        if ((i < 0) && (i != HTTP_PARSE_SSL))
        {
            break;
        }

printf("+++\turl_relay_f (3:1) s_addr %lu\n", (unsigned long)sap->sad.sa.sin_addr.s_addr);

#       if defined(HAVE_IFADDRS_H)
        /* Check loop to local address destenation */
        if (url_check_dst(&sap->sad, (http_parse_status_e)i))
        {
            __MACRO_ISSYSLOG__(errstr = errstrengine[20]);
            if (i != HTTP_PARSE_SSL)
            {
                (void) tcp_send_error(&sap->sac, CP http_header_508, __CSZ(http_header_508), NULL);
            }
            break;
        }
#       endif

printf("+++\turl_relay_f (4) %s\n", strerror(errno));

#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        __MACRO_TOSCREEN_URL__("","client request, %d bytes:\n\t[\n\t\t%s\n\t]", 'f', data.sz,
            ((i != HTTP_PARSE_SSL) ? data.str : CP STR_SSL_TEXT)
        );
#       else
        __MACRO_TOSCREEN_URL__("","read client request: %d bytes", 'f', data.sz);
#       endif

        /* End client parse request */

        /* Begin check blocking URL HTTP/HTTPS site */

printf("+++\turl_relay_f (5) %s\n", strerror(errno));

        __HASH_RLOCK(htulock,
            is_bl = (((htb) && (hash_tree_url_search(htb, (void*)&hparse, abs_url) != HASH_TREE_NOTFOUND)) ? 1 : 0)
        );

printf("+++\turl_relay_f (6) %s\n", strerror(errno));

        url_stage_prn(is_bl, 1, srv, &hparse);

printf("+++\turl_relay_f (7) %s\n", strerror(errno));

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

printf("+++\turl_relay_f (8) %s\n", strerror(errno));

            static char hbuf[1024] = {0};
            string_s hres = { CP hbuf, 0 };

            if (net_sa_to_dns((sa_socket_t*)&sap->sad, (string_s*)&hres) < 0)
            {
                break;
            }
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

            url_stage_prn(is_bl, 2, srv, &hparse);

        } while(0);

printf("+++\turl_relay_f (9) %s\n", strerror(errno));

        if (is_bl)
        {
            if (i == HTTP_PARSE_SSL)
            {
                break;
            }
#           if defined(DEBUG_SYSLOG)
            errstr = tcp_send_error(&sap->sac, blockr, errstrengine);
#           else
            (void) tcp_send_error(&sap->sac, blockr);
#           endif

#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("","send clent 403 response: %d bytes", 'f', blockr.sz);
#           endif
            break;
        }
        /* End check blocking URL HTTP/HTTPS site */

        /* Begin remote proxy setup */
        do {
            __MACRO_ISSYSLOG__(errstr = NULL);
            __MACRO_ISSYSLOG__(errno  = 0);
            is_bl = 0;

            if ((sap->sas.sp = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            {
                __MACRO_ISSYSLOG__(errstr = errstrengine[10]);
printf("+++\tsocket %s\n", strerror(errno));
                break;
            }
            if (net_sa_timeout(&sap->sas, 5) < 0)
            {
                __MACRO_ISSYSLOG__(errstr = errstrengine[11]);
printf("+++\tnet_sa_timeout %s\n", strerror(errno));
                break;
            }
            if (net_sock_unblock(sap->sas.sp) == -1)
            {
printf("+++\tnet_sock_unblock %s\n", strerror(errno));
                __MACRO_ISSYSLOG__(errstr = errstrengine[19]);
                break;
            }
            if (
                (connect(sap->sas.sp, (struct sockaddr *)&sap->sas.sa, sap->sas.sl) < 0) &&
                (errno != EINPROGRESS)
               )
            {
                __MACRO_ISSYSLOG__(errstr = errstrengine[12]);
printf("+++\tconnect %s\n", strerror(errno));
                break;
            }
printf("+++\tPOST !!! connect %s\n", strerror(errno));
            is_bl = 1;

        } while (0);

        if (!is_bl)
        {
            (void) tcp_send_error(&sap->sac, CP http_header_451, __CSZ(http_header_451), NULL);
#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("","send client 451 response: %d bytes", 'f', __CSZ(http_header_451));
#           endif
            break;
        }

#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        __MACRO_TOSCREEN_URL__(
            "","to remote proxy, fd:%d - [CONNECT %s:%u HTTP/1.0]", 'f',
            sap->sas.sp, inet_ntoa(sap->sad.sa.sin_addr), ntohs(sap->sad.sa.sin_port)
        );
#       endif


        if ((data.sz = snprintf(data.str, (__RBUF_SIZE - 1), "CONNECT %s:%u HTTP/1.0\r\n\r\n", inet_ntoa(sap->sad.sa.sin_addr), ntohs(sap->sad.sa.sin_port))) <= 0)
        {
printf("+++\tsnprintf error %s\n", strerror(errno));

            __MACRO_ISSYSLOG__(errstr = errstrengine[18]);
            break;
        }

        data.str[data.sz] = '\0';

printf("+++\tsnprintf: [%s]\n", data.str);

        if (tcp_sends(SOCK_SERVER, &sap->sas, &data) < 0)
//        if (dprintf(sap->sas.sp, "CONNECT %s:%u HTTP/1.0\r\n\r\n", inet_ntoa(sap->sad.sa.sin_addr), ntohs(sap->sad.sa.sin_port)) <= 0)
        {
printf("+++\ttcp_sends SOCK_SERVER error %s\n", strerror(errno));
            __MACRO_ISSYSLOG__(errstr = errstrengine[13]);
            break;
        }
        if ((data.sz = (int)tcp_recv(SOCK_SERVER, &sap->sas, data.str, (__RBUF_SIZE - 1), 0)) <= 0)
        {

printf("+++\ttcp_recv SOCK_SERVER error %s\n", strerror(errno));

            if (data.sz == SOCK_SRV_TIMEOUT)
            {
#               if defined(DEBUG_SYSLOG)
                if ((errstr = tcp_send_error(&sap->sac, CP http_header_408, __CSZ(http_header_408), NULL)) == NULL)
#               else
                if (tcp_send_error(&sap->sac, CP http_header_408, __CSZ(http_header_408), NULL) == NULL)
#               endif
                {
                    __MACRO_ISSYSLOG__(errstr = errstrengine[14]);
                }
#               if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
                __MACRO_TOSCREEN_URL__("","send clent 408 response: %d bytes", 'f', __CSZ(http_header_408));
#               endif
                break;
            }
            __MACRO_ISSYSLOG__(errstr = errstrengine[14]);
            break;
        }

        __MACRO_TOSCREEN_URL__("","read proxy server response: %d bytes", 'f', data.sz);

#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        data.str[data.sz] = '\0';
        __MACRO_TOSCREEN_URL__("","server proxy response:\n\t[\n\t\t%s\n\t]", 'f',
            ((i != HTTP_PARSE_SSL) ? data.str : CP STR_SSL_TEXT)
        );
#       endif

        if (http_check_response(&data) == HTTP_BAD_RESPONSE)
        {
            if (i == HTTP_PARSE_SSL)
            {
printf("+++\tHTTP_PARSE_SSL break %s\n", strerror(errno));

                break;
            }
#           if defined(DEBUG_SYSLOG)
            if ((errstr = tcp_send_error(&sap->sac, data, errstrengine)) == NULL)
#           else
            if (tcp_send_error(&sap->sac, data) == NULL)
#           endif
            {
printf("+++\ttcp_send_error %s\n", strerror(errno));
                __MACRO_ISSYSLOG__(errstr = errstrengine[15]);
            }
#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("","send clent returned proxy response: %d bytes", 'f', data.sz);
#           endif
            break;
        }
        /* End remote proxy setup */

        net_tcp_buffer(sap);

        /* Commented debug socket status */
#       if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
        net_proxy_print(sap, -100, __FILE__, __LINE__);
#       else
        __MACRO_TOSCREEN_URL__("","using tcp buffer size: %d", 'f', sap->tcpbs);
#       endif

printf("+++\ttcp_send SOCK_SERVER bytes %d\n", srv->rbuf.sz);

        if (tcp_send(SOCK_SERVER, &sap->sas, (char*)srv->rbuf.str, srv->rbuf.sz) < 0)
        {
printf("+++\ttcp_send SOCK_SERVER error %s\n", strerror(errno));
            __MACRO_ISSYSLOG__(errstr = errstrengine[17]);
            break;
        }

printf("+++\turl_relay_f normal end %s\n", strerror(errno));

        return 1;

    } while(0);

printf("+++\turl_relay_f error end %s\n", strerror(errno));

#   if defined(DEBUG_SYSLOG)
    if (errstr != NULL)
    {
        __MACRO_TOSYSLOG__(
            LOG_ERR,
            STR_URL_DEBUG "url_relay_f %s:%d -> Error: %s (%s)",
            CP srv->ipstr.ip,
            srv->ipstr.port,
            errstr,
            ((errno > 0) ? strerror(errno) : CP "-")
        );
    }
#   endif

    return 0;
}
FUNINLINE int url_init_pollclient(srv_socket_url_t *srv, srv_socket_set_t *sst)
{
#   if defined(HAVE_SPLICE)
    if (pipe(sst->pipe) < 0)
    {
        return 0;
    }
#   endif
    sst->bsz   = srv->sap->tcpbs;
    sst->ipstr = &srv->ipstr;
    return 1;
}
FUNINLINE void url_set_pollclient(socket_select_e sidx, srv_socket_set_t *sst, sa_socket_t *from, sa_socket_t *to, int flags, char tag)
{
    sst->tag   = tag;
    sst->sidx  = sidx;
    sst->flags = flags;
    sst->from  = from;
    sst->to    = to;
    sst->sbytes = sst->rbytes = 0;
#   if !defined(HAVE_SPLICE)
#      if defined(MSG_NOSIGNAL)
        sst->flags = flags | MSG_NOSIGNAL | MSG_DONTWAIT;
#      else
        sst->flags = flags | MSG_DONTWAIT;
#      endif
#   endif
}
static int url_relay_p(srv_socket_url_t *srv)
{
    struct pollfd pfd[] =
    {
#       if defined(HAVE_SPLICE)
        { .fd = srv->sap->sas.sp, .events = POLLIN | POLLERR | POLLHUP | POLLNVAL },
        { .fd = srv->sap->sac.sp, .events = POLLIN | POLLERR | POLLHUP | POLLNVAL }
#       else
        { .fd = srv->sap->sas.sp, .events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL },
        { .fd = srv->sap->sac.sp, .events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL }
#       endif
    };
    time_t tmout = 0UL;
    sa_proxy_t *sap = (sa_proxy_t*)srv->sap;
    srv_socket_set_t srvs = {0}, *sst = (srv_socket_set_t*)&srvs;
    __cleanup_set_free(srv_socket_set_t, srvsfree, sst);

    if (!url_init_pollclient(srv, sst))
    {
        __MACRO_TOSCREEN_URL__("","poll error init instance: %s", 'p',
            strerror(errno)
        );
        __POLL_CLOSED(srvsfree, 0);
    }

    while(*srv->isrun && *srv->isalive)
    {
        switch(poll(pfd, __NELE(pfd), HAVE_NET_TIMEOUT))
        {
            case POLL_EXPIRE:
            {
                break;
            }
            case POLL_ERROR:
            {
                if ((errno == EINTR) || (errno == EAGAIN))
                {
                    break;
                }
#               if defined(DEBUG_URL_SENDRCV)
                __MACRO_TOSCREEN_URL__("","fd: %d/%d - poll error: %s", 'p', pfd[SOCK_CLIENT].fd, pfd[SOCK_SERVER].fd, strerror(errno));
#               endif
                __POLL_CLOSED(srvsfree, 0);
            }
            default:
            {
#               if defined(DEBUG_URL_SENDRCV)
                net_poll_print(&pfd[SOCK_SERVER], __FILE__ " (" STR_URL_SRV " FD)", __LINE__);
                net_poll_print(&pfd[SOCK_CLIENT], __FILE__ " (" STR_URL_CLI " FD)", __LINE__);
#               endif

                if (
                    (pfd[SOCK_CLIENT].revents & POLLERR)  ||
                    (pfd[SOCK_SERVER].revents & POLLERR)
                   )
                {
                    __MACRO_TOSCREEN_URL__("","fd: %d - %s poll error/closed: %s", 'p',
                        ((pfd[SOCK_CLIENT].revents & POLLERR) ? pfd[SOCK_CLIENT].fd : pfd[SOCK_SERVER].fd),
                        ((pfd[SOCK_CLIENT].revents & POLLERR) ? STR_URL_SRV : STR_URL_CLI),
                        strerror(errno)
                    );
                    __POLL_CLOSED(srvsfree, 0);
                }
                else if (
                    (pfd[SOCK_CLIENT].revents & POLLHUP)  ||
                    (pfd[SOCK_CLIENT].revents & POLLNVAL) ||
                    (pfd[SOCK_SERVER].revents & POLLHUP)  ||
                    (pfd[SOCK_SERVER].revents & POLLNVAL)
                   )
                {
                    __POLL_CLOSED(srvsfree, 1);
                }
                else if ((pfd[SOCK_CLIENT].revents & POLLIN) || (pfd[SOCK_CLIENT].revents & POLLPRI))
                {
                    url_set_pollclient(
                        SOCK_CLIENT,
                        sst,
                        &sap->sac,
                        &sap->sas,
                        ((pfd[SOCK_CLIENT].revents & POLLPRI) ? MSG_OOB : 0),
                        'q'
                    );
                    switch(tcp_transfer(sst))
                    {
                        case SOCK_SRV_NEGATIVE:
                        case SOCK_CLI_NEGATIVE:
                        case SOCK_SRV_SHUT:
                        case SOCK_CLI_SHUT:
                        case SOCK_CLI_DATAEND:
                        {
                            __POLL_CLOSED(srvsfree, 1);
                        }
                        case SOCK_SRV_TIMEOUT:
                        case SOCK_CLI_TIMEOUT:
                        {
                            if (!url_session_timeout(&tmout))
                            {
                                __POLL_CLOSED(srvsfree, 1);
                            }
                            continue;
                        }
                        case SOCK_IOCTLERR:
                        {
                            url_poll_session_send_error(&sap->sac, CP http_header_400, __CSZ(http_header_400));
                            __POLL_CLOSED(srvsfree, 1);
                        }
                        default:
                        {
                            tmout = 0UL;
                            continue;
                        }
                    }
                }
                else if ((pfd[SOCK_SERVER].revents & POLLIN) || (pfd[SOCK_SERVER].revents & POLLPRI))
                {
                    url_set_pollclient(
                        SOCK_SERVER,
                        sst,
                        &sap->sas,
                        &sap->sac,
                        ((pfd[SOCK_SERVER].revents & POLLPRI) ? MSG_OOB : 0),
                        'r'
                    );
                    switch(tcp_transfer(sst))
                    {
                        case SOCK_SRV_NEGATIVE:
                        case SOCK_CLI_NEGATIVE:
                        case SOCK_SRV_SHUT:
                        case SOCK_CLI_SHUT:
                        case SOCK_SRV_DATAEND:
                        {
                            __POLL_CLOSED(srvsfree, 1);
                        }
                        case SOCK_SRV_TIMEOUT:
                        {
                            if (!url_session_timeout(&tmout))
                            {
                                url_poll_session_send_error(&sap->sac, CP http_header_408, __CSZ(http_header_408));
                                __POLL_CLOSED(srvsfree, 1);
                            }
                            continue;
                        }
                        case SOCK_CLI_TIMEOUT:
                        {
                            if (!url_session_timeout(&tmout))
                            {
                                __POLL_CLOSED(srvsfree, 1);
                            }
                            continue;
                        }
                        case SOCK_IOCTLERR:
                        {
                            url_poll_session_send_error(&sap->sac, CP http_header_523, __CSZ(http_header_523));
                            __POLL_CLOSED(srvsfree, 1);
                        }
                        default:
                        {
                            tmout = 0UL;
                            continue;
                        }
                    }
                }
                else
                {
#                   if defined(DEBUG_URL_SENDRCV)
                    __MACRO_TOSCREEN_URL__("","fd: %d/%d poll UNKNOWN event", 'p', pfd[SOCK_CLIENT].fd, pfd[SOCK_SERVER].fd);
#                   endif
                }
                break;
            }
        }
    }
    __POLL_CLOSED(srvsfree, 1);
}

void * url_relay_a(void *obj)
{
    if (obj == NULL)
    {
        __PTH_POOL_EXIT(NULL,-1);
    }

    srv_socket_url_t *srv = (srv_socket_url_t*)obj;
    sa_proxy_t       *sap = (sa_proxy_t*)srv->sap;
    __cleanup_set_free(srv_socket_url_t, srvfree, srv);

    if (sap == NULL)
    {
        __cleanup_notsupport_p(auto_freesrv_socket_url_t, srvfree);
        __PTH_POOL_EXIT(NULL,-1);
    }

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();
    signal(SIGPIPE, SIG_IGN);

    __MACRO_ISSYSLOG__(const char *errstr = NULL);

#   if (defined(DEBUG_NODAEMON) && defined(DEBUG_URL_INFO))
    {
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN_URL__("\n", "%s", 'a', tms.str);
    }
#   endif

    do {

        /* Begin client setup */
        __MACRO_ISSYSLOG__(errno  = 0);

#       if defined(HAVE_HTTP_TCPDACL) && (HAVE_HTTP_TCPDACL == 1)
        if (!acl_netwrap(&sap->sac, 0))
        {
            __MACRO_ISSYSLOG__(errstr = errstrengine[16]);
            break;
        }
#       endif

printf("*** url_relay_a (1) s_addr %lu\n", (unsigned long)sap->sad.sa.sin_addr.s_addr);


printf("*** url_relay_a (2) %s\n", strerror(errno));
        /* End client setup */

        /* Begin client/proxy recive/send */
        do
        {
            if (!url_relay_f(srv))
            {
printf("*** url_relay_a (3) %s\n", strerror(errno));
                break;
            }
printf("*** url_relay_a (4) s_addr %lu\n", (unsigned long)sap->sad.sa.sin_addr.s_addr);

            (void) url_relay_p(srv);
printf("*** url_relay_a (5) %s\n", strerror(errno));

        } while(0);
        /* End client/proxy recive/send */

printf("*** url_relay_a (6) %s\n", strerror(errno));

#       if (defined(DEBUG_NODAEMON) && (defined(DEBUG_URL_SENDRCV) || defined(DEBUG_URL_INFO)))
        {
            __MACRO_TOSCREEN_URL__("", "end session (%d)\n", 'a', errno);
        }
#       endif

printf("*** url_relay_a (7) %s\n", strerror(errno));

        url_poll_session_end(sap); errno = 0;
printf("*** url_relay_a (8) %s\n", strerror(errno));

        __cleanup_notsupport_p(auto_freesrv_socket_url_t, srvfree);
printf("*** url_relay_a (9) %s\n", strerror(errno));

        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
printf("*** url_relay_a (10) %s\n", strerror(errno));

        __PTH_POOL_EXIT(NULL,0);

    } while(0);

    url_poll_session_end(sap);

#   if defined(DEBUG_SYSLOG)
    if (errstr != NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_URL_DEBUG "url_relay_a -> %s:%d\tsession error: %s (%s)", CP srv->ipstr.ip, srv->ipstr.port, errstr, strerror(errno));
    }
#   endif

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __cleanup_notsupport_p(auto_freesrv_socket_url_t, srvfree);
    __PTH_POOL_EXIT(NULL,-1);
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

        srv_transport_t tr = { NULL, &sa2, (void**)&htu, &slo, (net_cb)url_relay_a, NULL, NULL, NULL };
        __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "%s", "Network URL event poll -> start");

        signal(SIGUSR2, &child_SIGUSR2);

        if (url_poll_event(&tr) < 0)
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
