
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
#include "st-revizor-dns.h"
#include "st-revizor-filter-dns-debug.h"
#include "../ipfw/st-revizor-ipfw-list.h"

static hash_table_t *htd = NULL;
static st_table_lock_t htdlock = {0};
volatile unsigned long ipn = 0;

#if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
volatile int thid = 0;
#endif

extern int isrun;
extern string_s insource[];
extern int inprocess[];

static void dns_srv_pending(srv_socket_dns_t *srv, size_t idx)
{
    __TABLE_WLOCK(srv->reqlock,
        size_t test = 0;
        st_sub_counter(srv->reqsize, test);
        if (idx < test)
        {
            __SYNC_MEM(srv->requests[idx] = srv->requests[test]);
        }
    );
}
static void __dns_ctors_init(void)
{
#   if defined(HAVE_DNS_TARGET_ENDIP) && (HAVE_DNS_TARGET_ENDIP > 0)
    __MACRO_TOSCREEN_UDP_DNS__("init DNS blocking target -> %lu", 'i', HAVE_DNS_TARGET_ENDIP);
    ipn =
        ((HAVE_DNS_TARGET_ENDIP & 0xff000000) >> 24) |
        ((HAVE_DNS_TARGET_ENDIP & 0x00ff0000) >>  8) |
        ((HAVE_DNS_TARGET_ENDIP & 0x0000ff00) <<  8) |
        ((HAVE_DNS_TARGET_ENDIP & 0x000000ff) << 24);
#   endif
}
#if defined(__X_BUILD_ASM86) && defined(__GNUC__)
    void (*initdnsmod)(void) ELF_SECTION_ATR(".ctors") = __dns_ctors_init;
#endif

void * dns_relay_q(void *obj)
{
#   if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
    int thisid, pktcnt = 0;
    {
        thisid = net_idset(&thid);
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN__("\n" STR_DNS_DEBUG "dns_relay_udp_q id:%d -> %s", thisid, tms.str);
    }
#   endif

    int r, is_bl = 0;
    dns_io_buf_u  rbuf;
    memset(&rbuf, 0, sizeof(dns_io_buf_u));

    srv_socket_dns_t    *srv = (srv_socket_dns_t*)obj;
    hash_table_t    *htb = (hash_table_t*)*((srv_socket_dns_t*)obj)->obj;

    sa_socket_t    sax;
    net_sa_default(&sax);

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    while(isrun)
    {
        if ((r = recvfrom(srv->sa1->sac.sp, rbuf.raw, sizeof(rbuf.raw), 0, (void*)&sax.sa, &sax.sl)) < 0)
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 'q', "recvfrom empty!");
            break;
        }
        else if (r < sizeof(rbuf.udphdr))
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 'q', "malformed recvfrom");
            continue;
        }
#       if (defined(DEBUG_DNS_PACKET) && defined(DEBUG_NODAEMON))
        dns_dump_udpreq(&rbuf.udphdr, r);
#       endif

        if (DNS_GET_QR(&rbuf.udphdr) != 0)
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 'q', "udp packet not a query");
            continue;
        }

#       if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
        pktcnt++;
#       endif

        __MACRO_TOSCREEN_UDP_DNS__("query from ip:%s port:%d", 'q',
            inet_ntoa(sax.sa.sin_addr),
            htons(sax.sa.sin_port)
        );
        __MACRO_TOSCREEN_UDP_DNS__("pkt:%d", 'q', pktcnt);

#       if defined(HAVE_DNS_TCPDACL) && (HAVE_DNS_TCPDACL == 1)

#          if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
        if (!acl_netwrap(&sax, thisid))
#          else
        if (!acl_netwrap(&sax, 0))
#          endif
        {
            goto loopq;
        }
#       endif

        if ((DNS_GET_OPCODE(&rbuf.udphdr) == 0) && (rbuf.udphdr.qcount != 0))
        {
            size_t l, n = htons(rbuf.udphdr.qcount);
            for (l = 0; l < n; l++)
            {
                dns_question_t q = {{0,},0,0};

                if (dns_get_question(&rbuf.udphdr, r, l, &q) < 0)
                {
                    __MACRO_TOSCREEN_UDP_DNS__("%s", 'q', "malformed Q section");
                    break;
                }
                __HASH_RLOCK(htdlock,
                    is_bl = (((char*)q.name == NULL) ? 1 :
                                (((htb) && (hash_table_dns_search(htb, (char*)q.name) != HASH_TREE_NOTFOUND)) ? 1 : 0)
                    );
                );
                if (is_bl)
                {
                    __MACRO_TOSCREEN_UDP_DNS__("+block -> (reject)  -- %s", 'q', q.name);
                    /* Fast client response if record is blocked and NXDOMAIN mode set */
#                   if (defined(HAVE_DNS_TARGET_NXDOMAIN) && (HAVE_DNS_TARGET_NXDOMAIN == 1))

                    DNS_SET_QR(&rbuf.udphdr, 1);
                    DNS_SET_OPCODE(&rbuf.udphdr, 0);
                    DNS_SET_RCODE(&rbuf.udphdr, 3);

                    if ((r = sendto(srv->sa1->sac.sp, &rbuf, r, 0, (void*)&sax.sa, sax.sl)) < 0)
                    {
#                       if defined(EAGAIN)
                        if ((errno == EAGAIN) || (errno == EINTR))
#                       elif defined(EWOULDBLOCK)
                        if ((errno == EWOULDBLOCK) || (errno == EINTR))
#                       endif
                        {
                            goto loopq;
                        }
                        __MACRO_TOSCREEN_UDP_DNS__("send client < failed with -> %s", 'q', strerror(errno));
                    }
                    goto loopq;
#                   endif
                }
                else
                {
                    __MACRO_TOSCREEN_UDP_DNS__("=pass -> (%04llx)    -- %s", 'q',(0xffff & srv->reqnext), q.name);
                }
            }
        }
        __TABLE_WLOCK(srv->reqlock,

            pending_req_t *req;

            if (srv->reqsize < __NELE(srv->requests))
            {
                size_t test;
                st_add_counter(srv->reqsize, test);
                __SYNC_MEM(req = srv->requests + test);
            }
            else
            {
                __SYNC_MEM(req = srv->requests);
            }
            __SYNC_MEM(
                st_add_counter(srv->reqnext, req->id_ext);
                req->id_int    = rbuf.udphdr.id;
                req->is_bl     = is_bl;
                rbuf.udphdr.id = (req->id_ext & 0xffff);
                memcpy((void*)&req->addr, (void*)&sax.sa, sax.sl);
            );
        );
        if ((r = send(srv->sa1->sas.sp, &rbuf, r, 0)) < 0)
        {
#           if defined(EAGAIN)
            if ((errno != EAGAIN) && (errno != EINTR))
#           elif defined(EWOULDBLOCK)
            if ((errno != EWOULDBLOCK) && (errno != EINTR))
#           endif
            {
                __MACRO_TOSCREEN_UDP_DNS__("DNS uplink server %s -> failed with: %s", 'q', HAVE_DNS_UPSRV, strerror(errno));
            }
            break;
        }

#       if ((defined(HAVE_DNS_TCPDACL) && (HAVE_DNS_TCPDACL == 1)) || \
            (defined(HAVE_DNS_TARGET_NXDOMAIN) && (HAVE_DNS_TARGET_NXDOMAIN == 1)))
        __label_set(loopq);
#       endif

    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_EXIT(NULL,0);
}
void * dns_relay_r(void *obj)
{
#   if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
    int thisid, pktcnt = 0;
    {
        thisid = net_idset(&thid);
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN__("\n" STR_DNS_DEBUG "dns_relay_udp_r id:%d -> %s", thisid, tms.str);
    }
#   endif

    int r;
    size_t i;
    pending_req_t   *req = NULL;
    srv_socket_dns_t    *srv = (srv_socket_dns_t*)obj;

    dns_io_buf_u     rbuf;
    memset(&rbuf, 0, sizeof(dns_io_buf_u));

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    while(isrun)
    {
        if ((r = recv(srv->sa1->sas.sp, rbuf.raw, sizeof(rbuf.raw), 0)) <= 0)
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 'r', "recv empty!");
            break;
        }
        else if (r < sizeof(rbuf.udphdr))
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 'r', "Relay packet size malformed");
            continue;
        }
        if (DNS_GET_QR(&rbuf.udphdr) != 1)
        {
            __MACRO_TOSCREEN_UDP_DNS__("Relay QR -> %04hx -- not a response", 'r', rbuf.udphdr.id);
            continue;
        }

#       if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
        pktcnt++;
#       endif

        __MACRO_TOSCREEN_UDP_DNS__("send to client ip:%s port:%d", 'r',
            inet_ntoa(srv->requests[0].addr.sin_addr),
            htons(srv->requests[0].addr.sin_port)
        );
        __MACRO_TOSCREEN_UDP_DNS__("pkt:%d", 'r', pktcnt);

#       if (defined(DEBUG_DNS_PACKET) && defined(DEBUG_NODAEMON))
        dns_dump_udpreq(&rbuf.udphdr, r);
#       endif

        int      rqb = 0;
        size_t   rqs = 0;
        uint16_t rqi = 0;

        sa_socket_t     sax;
        net_sa_default(&sax);

        __TABLE_RLOCK(srv->reqlock,
            __SYNC_MEM(
            for (i = 0, req = srv->requests; i < srv->reqsize && i < HAVE_NET_QUEUE; i++, req++)
            {
                if ((req->id_ext & 0xffff) == rbuf.udphdr.id) { break; }
            });
            if (req != NULL)
            {
                __SYNC_MEM(
                {
                    rqs = srv->reqsize;
                    rqi = req->id_int;
                    rqb = req->is_bl;
                    memcpy(&sax.sa, &req->addr, sax.sl);
                });
            }
        );
        if ((req == NULL) || (rqs == 0) || (rqs == i))
        {
            __MACRO_TOSCREEN_UDP_DNS__("Relay request size -> %04hx -- response from unknown query", 'r', rbuf.udphdr.id);
            continue;
        }
        rbuf.udphdr.id = rqi;

        if (rqb && (DNS_GET_RCODE(&rbuf.udphdr) == 0))
        {
#           if (defined(HAVE_DNS_TARGET_NXDOMAIN) && (HAVE_DNS_TARGET_NXDOMAIN == 1))

            DNS_SET_QR(&rbuf.udphdr, 1);
            DNS_SET_OPCODE(&rbuf.udphdr, 0);
            DNS_SET_RCODE(&rbuf.udphdr, 3);

#           else

            for (i = 0; i < rbuf.udphdr.acount; i++)
            {
                dns_rr_t a;
                if (!dns_get_answer(&rbuf.udphdr, r, i, &a) < 0)
                {
                    __MACRO_TOSCREEN_UDP_DNS__("Relay request -> %04hx -- malformed get answer A.%u section", 'r', rbuf.udphdr.id, i);
                    goto loopr;
                }
                if (a.type != 1) { continue; }
                if (a.len  != 4)
                {
                    __MACRO_TOSCREEN_UDP_DNS__("Relay request -> %04hx -- malformed length A.%u section", 'r', rbuf.udphdr.id, i);
                    goto loopr;
                }
                memcpy(a.data, (void*)&ipn, sizeof(unsigned long));
            }
#           endif

        }
        if ((r = sendto(srv->sa1->sac.sp, &rbuf, r, 0, (void*)&sax.sa, sax.sl)) < 0)
        {
#           if defined(EAGAIN)
            if ((errno == EAGAIN) || (errno == EINTR))
#           elif defined(EWOULDBLOCK)
            if ((errno == EWOULDBLOCK) || (errno == EINTR))
#           endif
            {
                __MACRO_TOSCREEN_UDP_DNS__("Relay send -> retry        -- %s", 'r', strerror(errno));
                continue;
            }
            __MACRO_TOSCREEN_UDP_DNS__("Relay send -> client error -- %s", 'r', strerror(errno));
        }

#       if (!defined(HAVE_DNS_TARGET_NXDOMAIN) || (HAVE_DNS_TARGET_NXDOMAIN == 0))
        __label_set(loopr);
#       endif

        dns_srv_pending(srv, i);
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_EXIT(NULL,0);
}
void * dns_relay_tcp(void *obj)
{
#   if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
    {
        time_t lt = time(NULL);
        string_s tms = { ctime(&lt), 0 };
        string_trim(&tms);
        __MACRO_TOSCREEN__("\n" STR_DNS_DEBUG "dns_relay_tcp -> %s", tms.str);
    }
#   endif

    int               r;
    srv_socket_dns_t *srv = (srv_socket_dns_t*)obj;

    dns_io_buf_u      rbuf;
    memset(&rbuf, 0, sizeof(dns_io_buf_u));

    sa_socket_t       sax;
    net_sa_default(&sax);

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    sax.sp = accept(srv->sa2->sac.sp, (struct sockaddr*)&sax.sa, &sax.sl);
    if (sax.sp == -1)
    {
        __MACRO_TOSCREEN_TCP_DNS__("%s", "accept error");
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }

#   if (defined(HAVE_DNS_TCPDACL) && (HAVE_DNS_TCPDACL == 1))
    if (!acl_netwrap(&sax, sax.sp))
    {
        __MACRO_TOSCREEN_TCP_DNS__("%s", "DNS: ACL wrapper block packets..");
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }
#   endif

    fcntl(sax.sp, F_SETFD, fcntl(sax.sp, F_GETFD) | FD_CLOEXEC);
    if ((r = net_sock_iserror(sax.sp)) != 0)
    {
        __MACRO_TOSCREEN_TCP_DNS__("DNS: [%d/%d] socket error %s",
            srv->sa2->sac.sp, sax.sp, strerror(r)
        );
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }
    if (net_sa_timeout(&sax, 7) < 0)
    {
        __MACRO_TOSCREEN_TCP_DNS__("DNS: Error socket set timeout: %s",
            strerror(errno)
        );
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }
    if ((r = recv(sax.sp, rbuf.raw, sizeof(rbuf.raw), 0)) <= 0)
    {
#       if defined(EAGAIN)
        if ((errno != EAGAIN) && (errno != EINTR))
#       elif defined(EWOULDBLOCK)
        if ((errno != EWOULDBLOCK) && (errno != EINTR))
#       endif
        {
            __MACRO_TOSCREEN_TCP_DNS__("DNS: %s", "Recv empty!");
        }
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }
    else if (r < sizeof(rbuf.tcphdr))
    {
        __MACRO_TOSCREEN_TCP_DNS__("DNS: %s", "Relay tcp request malformed");
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }
    if (DNS_GET_QR(&rbuf.tcphdr) != 0)
    {
        __MACRO_TOSCREEN_TCP_DNS__("DNS: %s", "Tcp packet not a query");
        __PTH_POOL_TCP_EXIT(sax.sp, 0);
    }

#   if (defined(DEBUG_DNS_PACKET) && defined(DEBUG_NODAEMON))
    __MACRO_TOSCREEN_TCP_DNS__(
        "Before packet  QR:%d OPCODE:%d RCODE:%d ID:%d | recive %d bytes",
        DNS_GET_QR(&rbuf.tcphdr),
        DNS_GET_OPCODE(&rbuf.tcphdr),
        DNS_GET_RCODE(&rbuf.tcphdr),
        rbuf.tcphdr.id, r
    );
    dns_dump_tcpreq(&rbuf.tcphdr, r);
#   endif

    DNS_SET_QR(&rbuf.tcphdr, 1);
    DNS_SET_OPCODE(&rbuf.tcphdr, 0);
    DNS_SET_RCODE(&rbuf.tcphdr, 4);

#   if (defined(DEBUG_DNS_PACKET) && defined(DEBUG_NODAEMON))
    __MACRO_TOSCREEN_TCP_DNS__(
        "After packet   QR:%d OPCODE:%d RCODE:%d ID:%d | recive %d bytes",
        DNS_GET_QR(&rbuf.tcphdr),
        DNS_GET_OPCODE(&rbuf.tcphdr),
        DNS_GET_RCODE(&rbuf.tcphdr),
        rbuf.tcphdr.id, r
    );
#   endif

    __MACRO_TOSCREEN_TCP_DNS__("DNS: recevie %d bytes, return reject", r);

    if ((r = send(sax.sp, &rbuf, r, 0)) < 0)
    {
#       if defined(EAGAIN)
        if ((errno != EAGAIN) || (errno != EINTR))
#       elif defined(EWOULDBLOCK)
        if ((errno != EWOULDBLOCK) && (errno != EINTR))
#       endif
        {
            __MACRO_TOSCREEN_TCP_DNS__("DNS: send error: %s", strerror(errno));
        }
    }
    __MACRO_TOSCREEN_TCP_DNS__("DNS: close & exit socket %d", sax.sp);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_POOL_TCP_EXIT(sax.sp, 0);
}
FUNEXPORT int dns_init_table(void)
{
    __BUFFER_CREATE__(bpath);
    __MACRO_SNPRINTF__(bpath, -1, "%s/%s", HAVE_WATCH_BACKUP, insource[IN_FILE_BLIST].str);

    if (!file_exists_fp(&bpath))
    {
        __MACRO_TOSCREEN__(STR_DNS_DEBUG "DNS: Not update hash table: %s -> source not found", bpath.str);
        return 0;
    }

    hash_table_t *hold, *htmp = NULL;
    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "DNS: Pid -> %d. Wait, compile blacklist..", getpid());

    /* clear dns redirect rule */
#   if defined(HAVE_DNS_CLEAR_RULE) && (HAVE_DNS_CLEAR_RULE == 1)
    ipfw_exec(IPFW_DNSDEL, NULL);
#   endif

#   if defined(HAVE_DNS_EMPTY_RUN) && (HAVE_DNS_EMPTY_RUN == 1)
    htmp = hash_table_dns_read(&bpath);
#   else
    while ((htmp = hash_table_dns_read(&bpath)) == NULL)
    {
        __MACRO_TOSCREEN_UDP_DNS__("%s", 't', "DNS: Hash table is null..");
        if (!isrun)
        {
            __MACRO_TOSCREEN_UDP_DNS__("%s", 't', "DNS: End wait hash table source.. Return..");
            return -1;
        }
        sleep(2);
    }
#   endif
    if (
        (htmp == NULL) || 
        ((htmp->arr == NULL) && (htmp->ele == NULL)) ||
        ((htmp->arridx == 0UL) && (htmp->eleidx == 0UL))
       )
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "DNS: BlackList is empty -> Backup %s not found?", HAVE_WATCH_BACKUP);
        hash_table_free(htmp);
#       if (!defined(HAVE_DNS_CLEAR_RULE) || (HAVE_DNS_CLEAR_RULE == 0))
        ipfw_exec(IPFW_DNSDEL, NULL);
#       endif
#       if defined(HAVE_DNS_EMPTY_RUN) && (HAVE_DNS_EMPTY_RUN == 0)
        return 0;
#       else
        return -1;
#       endif
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "DNS: BlackList -> %lu hosts, %lu wilcards hosts. Compiled done..", htmp->arridx, htmp->eleidx);
        /* add iptables dns redirect rule */
#       if defined(HAVE_DNS_CLEAR_RULE) && (HAVE_DNS_CLEAR_RULE == 1)
        ipfw_exec(IPFW_DNSADD, NULL);
#       else
        if (htd == NULL)
        {
            ipfw_exec(IPFW_DNSADD, NULL);
        }
#       endif
    }
    __HASH_WLOCK(htdlock,
        hold = htd; htd = htmp
    );
    hash_table_free(hold);
    {
        __BUILD_string_static(fpath, HAVE_WATCH_BACKUP "/" TFTP_FILE_STATDNS);
        __BUFFER_CREATE__(statup);
        __MACRO_SNPRINTF__(statup, 0, "%lu|%lu", htd->arridx, htd->eleidx);
        file_create_fp((string_s*)&fpath, &statup);
        watch_set_autoconfig();
    }
    return 0;
}
static void child_SIGUSR2(int sig)
{
    if (dns_init_table() != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_DNS_DEBUG "DNS: Not update hash table from signal -> %d", sig);
    }
}
FUNEXPORT int dns_filter(void)
{
#   if (!defined(HAVE_DNS_FILTER_ENABLE) || (HAVE_DNS_FILTER_ENABLE == 0))
    ipfw_exec(IPFW_DNSDEL, NULL);
    return -1;
#   endif

    int ret = 0;

    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "DNS: Blocking request return target: %s", HAVE_DNS_TARGET_USERIP);

    if (dns_init_table() != 0)
    {
        return -1;
    }
    __cleanup_set_free(hash_table_t, htfree, htd);

    sa_proxy_t sa1, sa2;
    string_s sup = { HAVE_DNS_UPSRV, 53 },
#   if defined(HAVE_DNS_BIND) && (HAVE_DNS_BIND == 1)
             slo = { CP HAVE_IPFW_GW, HAVE_DNS_CLPORT };
#   else
             slo = { NULL, HAVE_DNS_CLPORT };
#   endif

    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "Using DNS remote server -> %s:%d", sup.str, sup.sz);
    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "Using DNS client listen -> %s:%d",
        ((slo.str == NULL) ? CP "0.0.0.0" : slo.str),
        slo.sz
    );

    do {

        errno = 0;
        if (net_udp_proxy_init(&sa1, &sup, &slo) < 0)
        {
            ret = -1;
            break;
        }
        if ((net_sa_timeout(&sa1.sac, 10) < 0) || (net_sa_timeout(&sa1.sas, 10) < 0))
        {
            ret = -1;
            break;
        }
#       if defined(HAVE_DNS_TCP_REJECT) && (HAVE_DNS_TCP_REJECT == 1)
        if (net_tcp_proxy_init(&sa2, NULL, &slo) < 0)
        {
            ret = -1;
            break;
        }
        if (net_sa_timeout(&sa2.sac, 14) < 0)
        {
            ret = -1;
            break;
        }
#       else
        net_default_proxy_init(&sa2);
#       endif

#       if !defined(__X_BUILD_ASM86) || !defined(__GNUC__)
        __dns_ctors_init();
#       endif

        srv_transport_t tr = { &sa1, &sa2, (void**)&htd, &slo, (net_cb)dns_relay_q, (net_cb)dns_relay_r, (net_cb)dns_relay_tcp, NULL};
        __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "%s", "Network DNS event poll -> start");

        signal(SIGUSR2, &child_SIGUSR2);

        if (dns_event_poll(&tr) < 0)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_DNS_DEBUG "Network DNS event poll -> end : %s", strerror(errno));
            ret = -1;
            break;
        }

    } while(0);

    if (ret == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_DNS_DEBUG "%s", "Network DNS proxy init error");
    }
    __cleanup_notsupport_p(auto_freehash_table_t, htfree);
    return ret;
}
