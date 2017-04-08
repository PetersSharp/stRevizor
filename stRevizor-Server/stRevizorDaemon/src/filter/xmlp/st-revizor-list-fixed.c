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
#include "st-revizor-xml-parser.h"

#define __BUFF_FIXED_MAX(A,B) \
    ((sizeof(A) > sizeof(B)) ? sizeof(A) : sizeof(B))

static string_s fixblock[] = {
#if defined(HAVE_XML_PARSER_FIX_BLACKIP)
    { __ADD_string_s(HAVE_XML_PARSER_FIX_BLACKIP) },
#else
    { NULL, 0 },
#endif
#if defined(HAVE_XML_PARSER_FIX_DNS)
    { __ADD_string_s(HAVE_XML_PARSER_FIX_DNS) },
#else
    { NULL, 0 },
#endif
#if defined(HAVE_XML_PARSER_FIX_URL)
    { __ADD_string_s(HAVE_XML_PARSER_FIX_URL) },
#else
    { NULL, 0 },
#endif
#if defined(HAVE_XML_PARSER_FIX_WHITEIP)
    { __ADD_string_s(HAVE_XML_PARSER_FIX_WHITEIP) }
#else
    { NULL, 0 }
#endif
};

static void fixed_list_ele_(xml_ele_t *ele, xml_ele_e idx, char *src)
{
    switch(idx)
    {
        case XML_ELE_WHITE:
        {
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I, CP "W", src);
            break;
        }
        case XML_ELE_IP:
        {
            __BUILD_string_s(srci, src);
            if (arr_ip_add(&ele->lip_ip, &srci, CP "IP fixed BlackList") == 0UL)
            {
                break;
            }
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I,  CP "R", src);
            if (strchr(src, '/') != NULL)
            {
                __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I,  CP "B", src);
            }
            else
            {
                __LOOP_WDATA(ele->fp, __LOOP_FORMAT_BF, CP "B", src);
            }
            break;
        }
        case XML_ELE_DNS:
        {
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_D, CP "D", src);
            break;
        }
        case XML_ELE_URL:
        {

            xml_uri_t uri = {0};
            string_s srci = { strdup(src), strlen(src) }; 

            if (uri_parse(&uri, &srci) == 0)
            {
                plist_add_url(ele, &uri);
            }
            free(srci.str);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void fixed_list_parse_(xml_ele_t *ele, xml_ele_e idx)
{
    if (idx > XML_ELE_WHITE)
    {
#       if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> XML fixed list parse: out of range index [%d]",
            idx
        );
#       endif
        return;
    }
    if ((fixblock[idx].str == NULL) || (fixblock[idx].sz == 0))
    {
        return;
    }
    if (strchr(fixblock[idx].str, ',') != NULL)
    {
        char *t, rbuf[(fixblock[idx].sz + 1)], *c = CP rbuf;
        memcpy(&rbuf, fixblock[idx].str, fixblock[idx].sz);
        rbuf[fixblock[idx].sz] = '\0';

        while ((t = strsep((char**)&c, ",")) != NULL)
        {
            fixed_list_ele_(ele, idx, t);
        }
    }
    else
    {
        fixed_list_ele_(ele, idx, fixblock[idx].str);
    }
}

void plist_fixed_list(xml_ele_t *ele)
{
    for (int i = 0; i < XML_ELE_CNT; i++)
    {
        fixed_list_parse_(ele, (xml_ele_e)i);
    }
}
