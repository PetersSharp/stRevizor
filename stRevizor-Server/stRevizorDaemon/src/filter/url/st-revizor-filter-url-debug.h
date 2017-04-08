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

#ifndef REVIZOR_URL_DEBUG_H
#define REVIZOR_URL_DEBUG_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../../config.h"
#endif

#include "../../st-revizor-watch.h"

#if defined(DEBUG_URL_INFO)
#   undef DEBUG_URL_INFO
#endif

#if (defined(HAVE_HTTP_DEBUGI) && (HAVE_HTTP_DEBUGI == 1))
#   define DEBUG_URL_INFO 1
#endif

#if defined(DEBUG_URL_PACKET)
#   undef DEBUG_URL_PACKET
#endif

#if (defined(HAVE_HTTP_DEBUGP) && (HAVE_HTTP_DEBUGP == 1))
#   define DEBUG_URL_PACKET 1
#endif

#if defined(DEBUG_URL_PRINT)
#   undef DEBUG_URL_PRINT
#endif

#if defined(DEBUG_URL_SENDRCV)
#   undef DEBUG_URL_SENDRCV
#endif

#if (defined(DEBUG_PRINT) && (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)))
#   define DEBUG_URL_PRINT 1
#endif

#if (defined(DEBUG_URL_PRINT) && (defined(HAVE_HTTP_DEBUGC) && (HAVE_HTTP_DEBUGC == 1)))
#   define DEBUG_URL_SENDRCV 1
#endif

#if defined(__MACRO_TOSCREEN_URL__)
#   undef __MACRO_TOSCREEN_URL__
#endif

#if (defined(DEBUG_URL_INFO) && defined(DEBUG_URL_PRINT))
#   define __MACRO_TOSCREEN_URL__(A,B,C,...) \
    __MACRO_TOSCREEN__( \
        A STR_URL_DEBUG "url_relay_%c %s:%d -> " B, C, \
        CP srv->ipstr.ip, srv->ipstr.port, __VA_ARGS__ \
    )
#else
#   define __MACRO_TOSCREEN_URL__(...)
#endif

#if (defined(DEBUG_URL_INFO) && defined(DEBUG_NODAEMON))
#   define __MACRO_POLLSCREEN_URL__(A, B, C) \
        __MACRO_TOSCREEN__(STR_POLL_EV_DEBUG, "URL", '*', "POLLIN", -1, A, B, C, "(no error)")
#else
#   define __MACRO_POLLSCREEN_URL__(...)
#endif

#endif
