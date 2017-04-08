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

#ifndef REVIZOR_DNS_DEBUG_H
#define REVIZOR_DNS_DEBUG_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include "../../st-revizor-watch.h"

#if defined(DEBUG_DNS_INFO)
#   undef DEBUG_DNS_INFO
#endif

#if (defined(HAVE_DNS_DEBUGI) && (HAVE_DNS_DEBUGI == 1))
#   define DEBUG_DNS_INFO 1
#endif

#if defined(DEBUG_DNS_PACKET)
#   undef DEBUG_DNS_PACKET
#endif

#if (defined(HAVE_DNS_DEBUGP) && (HAVE_DNS_DEBUGP == 1))
#   define DEBUG_DNS_PACKET 1
#endif

#if defined(DEBUG_DNS_PRINT)
#   undef DEBUG_DNS_PRINT
#endif

#if (defined(DEBUG_PRINT) && (defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)))
#   define DEBUG_DNS_PRINT 1
#endif

#if defined(__MACRO_TOSCREEN_UDP_DNS__)
#   undef __MACRO_TOSCREEN_UDP_DNS__
#endif

#if defined(__MACRO_TOSCREEN_TCP_DNS__)
#   undef __MACRO_TOSCREEN_TCP_DNS__
#endif

#if (defined(DEBUG_DNS_INFO) && defined(DEBUG_DNS_PRINT))
#   define __MACRO_TOSCREEN_UDP_DNS__(A,B,...) __MACRO_TOSCREEN__(STR_DNS_DEBUG "dns_relay_udp_%c id:%d -> " A, B, thisid, __VA_ARGS__)
#   define __MACRO_TOSCREEN_TCP_DNS__(A,...)   __MACRO_TOSCREEN__(STR_DNS_DEBUG "dns_relay_tcp %s:%d -> " A, inet_ntoa(sax.sa.sin_addr), ntohs(sax.sa.sin_port), __VA_ARGS__)
#else
#   define __MACRO_TOSCREEN_UDP_DNS__(...)
#   define __MACRO_TOSCREEN_TCP_DNS__(...)
#endif

#if (defined(DEBUG_DNS_INFO) && defined(DEBUG_NODAEMON))
#   define __MACRO_POLLSCREEN_DNS__(A, B, C, D) \
        __MACRO_TOSCREEN__(STR_POLL_EV_DEBUG, "DNS", '*', "POLLIN", A, B, C, D, "(no error)")
#else
#   define __MACRO_POLLSCREEN_DNS__(...)
#endif

/*
//    Check block:

#   if defined(DEBUG_PRINT)
#   error "defined(DEBUG_PRINT)"
#   else
#   error "NOT defined(DEBUG_PRINT)"
#   endif

#   if defined(DEBUG_DNS_PRINT)
#   error "defined(DEBUG_DNS_PRINT)"
#   else
#   error "NOT defined(DEBUG_DNS_PRINT)"
#   endif

*/

#endif

