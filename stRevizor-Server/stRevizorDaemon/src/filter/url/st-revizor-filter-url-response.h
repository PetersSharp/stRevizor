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

#ifndef REVIZOR_URL_RESPONSE_H
#define REVIZOR_URL_RESPONSE_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../../config.h"
#endif

#if !defined(HAVE_BUILD_VER)
#    define HAVE_BUILD_VER "x.x.x.x"
#endif

#if defined(__URL_ERR_ENGINE)

extern const unsigned char _binary_______src_filter_url_url_response_st_revizor_filter_url_response_tobin_start;
extern const unsigned char _binary_______src_filter_url_url_response_st_revizor_filter_url_response_tobin_end;
static const unsigned char *htmlrs = &_binary_______src_filter_url_url_response_st_revizor_filter_url_response_tobin_start;
static const unsigned char *htmlre = &_binary_______src_filter_url_url_response_st_revizor_filter_url_response_tobin_end;

char http_header_400[] =
    "HTTP/1.0 400 Bad Request\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Bad Request, data empty.\r\nNot completely, abort..\r\n";

char http_header_403[] =
    "HTTP/1.0 403 Forbidden\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n" \
    "Date: %s\r\n" \
    "Size: %d\r\n\r\n";

char http_header_408[] =
    "HTTP/1.0 408 Request Timeout\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n" \
    "Remote proxy server timed out waiting for this request.\r\nNot completely, abort..\r\n";

char http_header_451[] =
    "HTTP/1.0 451 Unavailable For Legal Reasons\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Unavailable For Legal Reasons, web page censored by a government.\r\nNot completely, abort..\r\n";

char http_header_503[] =
    "HTTP/1.0 503 Service Unavailable\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Remote proxy server unavailable.\r\nNot completely, abort..\r\n";

char http_header_508[] =
    "HTTP/1.0 508 Loop Detected\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Loop Detected, using SNAT from SSL connection?\r\nAbort..\r\n";

char http_header_523[] =
    "HTTP/1.0 523 Origin Is Unreachable\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Origin Is Unreachable, data empty.\r\nNot completely, abort..\r\n";

#undef __URL_ERR_ENGINE
#endif

#if defined(__URL_ERR_LOOP)

char http_header_500pf[] =
    "HTTP/1.0 500 Internal Server Error\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Internal Server Error, poll full.\r\nNot completely, abort..\r\n";

char http_header_500me[] =
    "HTTP/1.0 500 Internal Server Error\r\n" \
    "X-Proxy: stRevizor-" HAVE_BUILD_VER "\r\n\r\n" \
    "Internal Server Error, client not allocate data.\r\nNot completely, abort..\r\n";
#undef __URL_ERR_LOOP
#endif

#endif
