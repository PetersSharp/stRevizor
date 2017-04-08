
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
#include "st-revizor-dns.h"

#include <pthread.h>
#include <signal.h>

hash_table_t *ht = NULL;
hash_table_lock_t htlock = {0};

extern int isrun;
extern string_s insource[];
extern int inprocess[];
extern nfbin_t binpath;

static void dns_srv_pending(srv_socket_t *srv, int i)
{
    if (i < --srv->pending)
    {
        srv->requests[i] = srv->requests[srv->pending];
    }
}
void * dns_relay_q(void *obj)
{
    int r, is_bl;
    pending_req_t *req;
    dns_question_t q;
    dns_io_buf_t  rbuf;
    memset(&rbuf, 0, sizeof(dns_io_buf_t));

    srv_transport_t *tr  = (srv_transport_t*)obj;
    srv_socket_t    *srv = (srv_socket_t*)tr->srv;
    hash_table_t    *htb = (hash_table_t*)*tr->obj;

    sa_socket_t    sax;
    net_sa_default(&sax);

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    while(1)
    {
        if ((r = recvfrom(srv->skc, rbuf.raw, sizeof(rbuf.raw), 0, (void*)&sax.sa, &sax.sl)) < 0) { break; }
        else if (r < sizeof(rbuf.hdr))
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "malformed");
#           endif
            continue;
        }
        else if (DNS_GET_QR(&rbuf.hdr) != 0)
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "not a query");
#           endif
            continue;
        }
        is_bl = 0;
#       if defined(HAVE_DNS_DEBUGP) && (HAVE_DNS_DEBUGP == 1)
        dns_dump_response(&rbuf.hdr, r);
#       endif

        if (DNS_GET_OPCODE(&rbuf.hdr) == 0 && rbuf.hdr.qcount != 0)
        {
            size_t i, n = htons(rbuf.hdr.qcount);
            for (i = 0; i < n; i++)
            {
                if (dns_get_question(&rbuf.hdr, r, i, &q) < 0)
                {
#                   if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
                    __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "malformed Q section");
#                   endif
                    break;
                }
            }
            hash_read_lock(&htlock);
            if ((htb) && (hash_search_uniq(htb, q.name) != 0UL)) { is_bl = 1; }
            hash_read_unlock(&htlock);

#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__("[DNS filter]: %s %04llx -- %s",
                ((is_bl) ? "+block -> " : "=pass  -> "),
                0xffff & srv->id_next, q.name
            );
#           endif
        }
        if (srv->pending < __NELE(srv->requests))
        {
            req = srv->requests + srv->pending++;
        }
        else
        {
            req = srv->requests;
        }
        req->addr    = sax.sa;
        req->id_ext  = srv->id_next++;
        req->id_int  = rbuf.hdr.id;
        req->is_bl   = is_bl;
        rbuf.hdr.id  = (req->id_ext & 0xffff);
        if ((r = send(srv->sks, &rbuf, r, 0)) < 0)
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "srv < failed with -> %s", strerror(errno));
#           endif
            break;
        }
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_EXIT(tr,0);
}
void * dns_relay_r(void *obj)
{
    int r;
    size_t i;
    pending_req_t   *req;
    srv_transport_t *tr   = (srv_transport_t*)obj;
    srv_socket_t    *srv  = (srv_socket_t*)tr->srv;
    dns_io_buf_t     rbuf;
    memset(&rbuf, 0, sizeof(dns_io_buf_t));

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    while(1)
    {
        if ((r = recv(srv->sks, rbuf.raw, sizeof(rbuf.raw), 0)) <= 0) { break; }
        else if (r < sizeof(rbuf.hdr))
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "relay res malformed");
#           endif
            continue;
        }
        else if (DNS_GET_QR(&rbuf.hdr) != 1)
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "relay res -> %04hx -- not a response", rbuf.hdr.id);
#           endif
            continue;
        }
#       if defined(HAVE_DNS_DEBUGP) && (HAVE_DNS_DEBUGP == 1)
        dns_dump_response(&rbuf.hdr, r);
#       endif

        for (i = 0, req = srv->requests; i < srv->pending; i++, req++)
        {
            if ((req->id_ext & 0xffff) == rbuf.hdr.id) { break; }
        }
        if (i == srv->pending)
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "relay res -> %04hx -- response from unknown query", rbuf.hdr.id);
#           endif
            continue;
        }
        rbuf.hdr.id = req->id_int;
        if (req->is_bl && (DNS_GET_RCODE(&rbuf.hdr) == 0))
        {
            for (i = 0; i < rbuf.hdr.acount; i++)
            {
                dns_rr_t a;
                if (!dns_get_answer(&rbuf.hdr, r, i, &a) < 0)
                {
#                   if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
                    __MACRO_TOSCREEN__(STR_DNS_DEBUG "relay res -> %04hx -- malformed A.%u section", rbuf.hdr.id, i);
#                   endif
                    dns_srv_pending(srv, i);
                    __PTH_EXIT(tr,-1);
                }
                if (a.type != 1) { continue; }
                else if (a.len != 4)
                {
#                   if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
                    __MACRO_TOSCREEN__(STR_DNS_DEBUG "relay res -> %04hx -- malformed A.%u section", rbuf.hdr.id, i);
#                   endif
                    dns_srv_pending(srv, i);
                    __PTH_EXIT(tr,-1);
                }
                *(uint32_t*)a.data = 0;
            }
        }
        if ((r = sendto(srv->skc, &rbuf, r, 0, (void*)&req->addr, sizeof req->addr)) < 0)
        {
#           if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "relay res -> client < failed with -- %s", strerror(errno));
#           endif
        }
        dns_srv_pending(srv, i);
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_EXIT(tr,0);
}
int dns_init_table(void)
{
    __BUFFER_CREATE__(bpath);
    __MACRO_SNPRINTF__(bpath, -1, "%s/%s", HAVE_WATCH_BACKUP, insource[7].str);
    hash_table_t *hold, *htmp = NULL;

    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "pid -> %d. Wait, compile blacklist..", getpid());

#   if defined(HAVE_DNS_EMPTY_RUN) && (HAVE_DNS_EMPTY_RUN == 1)
    htmp = hash_read(&bpath);
#   else
    // clear dns redirect rule
    main_module_dns(0);

    while ((htmp = hash_read(&bpath)) == NULL)
    {
#       if defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1)
        __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "hash table is null..");
#       endif
        if (!isrun)
        {
            __MACRO_TOSCREEN__(STR_DNS_DEBUG "%s", "end wait hash table source.. Return..");
            return -1;
        }
        sleep(2);
    }
#   endif
    if ((htmp == NULL) || (htmp->arr == NULL) || (htmp->idx == 0UL))
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "blacklist is empty -> Backup %s not found?", HAVE_WATCH_BACKUP);
        hash_free(htmp);
#       if defined(HAVE_DNS_EMPTY_RUN) && (HAVE_DNS_EMPTY_RUN == 0)
        return 0;
#       endif
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "blacklist -> %lu hosts. Compiled done..", htmp->idx);
        // add dns redirect rule
        main_module_dns(1);
    }
    hash_write_lock(&htlock);
    {
        hold = ht;
        ht   = htmp;
    }
    hash_write_unlock(&htlock);
    hash_free(hold);
    return 0;
}

int dns_filter(void)
{
    int ret = 0;

    if (dns_init_table() != 0)
    {
        return -1;
    }

    srv_socket_t  srv = {0};
    string_s sup = { HAVE_DNS_UPSRV, 53 },
             slo = { NULL, HAVE_DNS_CLPORT };

    if (net_udp_proxy_init(&srv, sup, slo) >= 0)
    {
        srv_transport_t tr = { &srv, (void**)&ht, (net_cb)dns_relay_q, (net_cb)dns_relay_r, 0};

        if (net_event_poll(&tr) < 0)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_DNS_DEBUG "network event poll end -> %s", strerror(errno));
            ret = -1;
        }
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_DNS_DEBUG "%s", "network udp proxy init error");
        ret = -1;
    }

    hash_free(ht);
    return ret;
}
