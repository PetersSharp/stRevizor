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

FUNEXPORT int acl_netwrap(sa_socket_t *sax, int id)
{
#if (defined(HAVE_DNS_TCPDACL) && (HAVE_DNS_TCPDACL == 1)) || \
    (defined(HAVE_HTTP_TCPDACL)&& (HAVE_HTTP_TCPDACL == 1))

#include <tcpd.h>
    struct request_info rq = {0};
    request_init(&rq, RQ_DAEMON, PROCID, RQ_CLIENT_SIN, &sax->sa, 0);
    fromhost(&rq);
    if (!hosts_access(&rq))
    {
        refuse(&rq);
        __BUFFER_CREATE__(sip);
        net_sa_getip((const struct sockaddr*)&sax->sa, &sip);
        __MACRO_TOSYSLOG__(LOG_AUTHPRIV, STR_ACL_DEBUG "Access denied -> %s (%d), id:%d", sip.str, sip.sz, id);
        return 0;
    }
#endif
    return 1;
}
