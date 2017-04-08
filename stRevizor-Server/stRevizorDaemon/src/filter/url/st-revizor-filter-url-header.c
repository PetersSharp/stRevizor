
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
#include "../st-revizor-net.h"
#include "st-revizor-filter-url-header.h"
#include "st-revizor-filter-url-debug.h"

#define HTTP_PORT_STRING "80"
#define HTTPS_PORT_STRING "443"

#define __HTTP_FIELD_ADD(A,B,C) \
    A.sz  = C; \
    A.sz  = (((A.sz > __BUFFER_SIZE__) || (A.sz <= 0)) ? strlen(B) : A.sz); \
    A.str = (B)

static const string_s hmethod[] = {
    { __ADD_string_s("GET ") },
    { __ADD_string_s("PUT ") },
    { __ADD_string_s("POST ") },
    { __ADD_string_s("PATCH ") },
    { __ADD_string_s("HEAD ") },
    { __ADD_string_s("TRACE ") },
    { __ADD_string_s("OPTIONS ") },
    { __ADD_string_s("DELETE ") },
    { __ADD_string_s("CONNECT ") }
};

static const string_s htoken[] = {
    { __ADD_string_s("Port: ") },
    { __ADD_string_s("Host: ") },
    { __ADD_string_s("Url: ") },
    { __ADD_string_s("Origin: ") },
    { __ADD_string_s("Search limit: ") },
    { __ADD_string_s("Method req: ") }
};

FUNEXPORT void http_init_parse_s(http_parse_s *hps)
{
    memset(hps, 0, sizeof(http_parse_s));
}
FUNEXPORT int http_check_response(string_s *s)
{
    do {
        if ((s == NULL) || (s->str == NULL) || (s->sz < 12)) { break; }
        if (
            (s->str[0]  == 'H') &&
            (s->str[1]  == 'T') &&
            (s->str[2]  == 'T') &&
            (s->str[3]  == 'P') &&
            (s->str[4]  == '/') &&
            (s->str[9]  == '2') &&
            (s->str[10] == '0') &&
            (s->str[11] == '0')
           )
        {
            /* return HTTP version 1.0 = 10, 1.1 == 11, 2.0 = 20 */
#           if defined(__GNUC__) || defined(__GCC__)
            return __extension__ (
                {
                    int i = ((s->str[5] == '1') ? 10 : ((s->str[5] == '2') ? 20 : 0));
                    i += ((s->str[7] == '1') ? 1 : 0); i;
                }
            );
#           else
            return ((s->str[7] == '1') ? 11 : 10);
#           endif
        }
    } while(0);

    return HTTP_BAD_RESPONSE;
}
FUNEXPORT int http_parse_request(string_s *s, http_parse_s *h)
{
    if ((s == NULL) || (s->str == NULL) || (s->sz <= 0))
    {
        return HTTP_HEADER_EMPTY;
    }
    int  oid = -1, mid = -1;
    char *c, *cc, *t = NULL;
    string_s stb = {0};
    c = cc = (char*)s->str;

    switch(c[0])
    {
        case 'G':
        {
            mid = HTTP_METHOD_GET;
            break;
        }
        case 'P':
        {
            if (c[1] == 'U')
            {
                mid = HTTP_METHOD_PUT;
            }
            else if (c[1] == 'O')
            {
                mid = HTTP_METHOD_POST;
            }
            else if (c[1] == 'A')
            {
                mid = HTTP_METHOD_PATCH;
            }
            else
            {
                return HTTP_HEADER_ERROR;
            }
            break;
        }
        case 'H':
        {
            mid = HTTP_METHOD_HEAD;
            break;
        }
        case 'T':
        {
            mid = HTTP_METHOD_TRACE;
            break;
        }
        case 'O':
        {
            mid = HTTP_METHOD_OPTIONS;
            break;
        }
        case 'D':
        {
            mid = HTTP_METHOD_DELETE;
            break;
        }
        case 'C':
        {
#       if defined(HAVE_HTTP_ALLOW_CONNECT)
            /* enable incoming CONNECT proxy method */
            mid = HTTP_METHOD_CONNECT;
            break;

#       else
            return HTTP_HEADER_NOTIMPL;
#       endif
        }
        default:
        {
            return HTTP_HEADER_ERROR;
        }
    }
    if (
        (mid < 0) ||
        (mid >= __NELE(hmethod)) ||
        (memcmp(cc, hmethod[mid].str, hmethod[mid].sz) != 0)
       )
    {
        return HTTP_HEADER_ERROR;
    }
    while ((t = strsep((char**)&c, "\r\n")) != NULL)
    {
        switch(t[0])
        {
            case 'H':
            {
                oid = HTTP_FIELD_HOST;
                break;
            }
            /*
            case 'O':
            {
                oid = HTTP_FIELD_ORIGIN;
                break;
            }
            */
            default:
            {
                continue;
            }
        }
        if (
            (oid >= 0) &&
            (oid < __NELE(htoken)) &&
            (memcmp(t, htoken[oid].str, htoken[oid].sz) == 0)
           )
        {
            t  = ((char *)t + htoken[oid].sz);
            __HTTP_FIELD_ADD(
                h->field[oid],
                t,
                (int)(c - t - 1)
            );
        }
        oid = -1;
    }
    /* Parse HTTP_FIELD_URL */
    c  = ((char *)cc + hmethod[mid].sz);
    if ((t = strsep((char**)&c, " ")) == NULL)
    {
        return HTTP_HEADER_ERROR;
    }
    if (
        ((int)(c - t - 1) > 1) &&
        (t[0] == '/') && (t[1] == '/')
       )
    {
        int pos = string_poschar((char*)(t + 1), '/');
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_URL],
            (char*)(t + pos + 1),
            (int)(c - t - pos - 2)
        );
    }
    else
    {
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_URL],
            t,
            (int)(c - t - 1)
        );
    }
    switch (mid)
    {
#       if defined(HAVE_HTTP_ALLOW_CONNECT)
        case HTTP_METHOD_CONNECT:
        {
            memcpy(&stb, &h->field[HTTP_FIELD_URL], sizeof(string_s));
            h->field[HTTP_FIELD_URL].str = NULL;
            h->field[HTTP_FIELD_URL].sz  = 0;
            break;
        }
#       endif
        default:
        {
            memcpy(&stb, &h->field[HTTP_FIELD_HOST], sizeof(string_s));
            break;
        }
    }
    /* Parse HTTP_FIELD_HOST && HTTP_FIELD_PORT */
    if ((stb.str != NULL) && ((t = strchr(stb.str, ':')) != NULL))
    {
        t  = ((char *)t + 1);
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_PORT],
            t,
            (int)(stb.str - (t - stb.str - 1))
        );
        if ((t = strsep((char**)&stb.str, ":")) != NULL)
        {
            __HTTP_FIELD_ADD(
                h->field[HTTP_FIELD_HOST],
                t,
                (int)(stb.str - t - 1)
            );
        }
        else
        {
            return HTTP_HEADER_ERROR;
        }
    }
    else
    {
#       if defined(HAVE_HTTP_ALLOW_CONNECT)
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_PORT],
            ((mid == HTTP_METHOD_CONNECT) ? CP HTTPS_PORT_STRING : CP HTTP_PORT_STRING),
            ((mid == HTTP_METHOD_CONNECT) ? __CSZ(HTTPS_PORT_STRING) : __CSZ(HTTP_PORT_STRING))
        );
#       else
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_PORT],
            CP HTTP_PORT_STRING,
            __CSZ(HTTP_PORT_STRING)
        );
#       endif
    }
    /* Set formating URL */
    if (
        (h->field[HTTP_FIELD_URL].str != NULL) &&
        (h->field[HTTP_FIELD_HOST].str != NULL) &&
        ((t = strstr(h->field[HTTP_FIELD_URL].str, h->field[HTTP_FIELD_HOST].str)) != NULL)
       )
    {
        t = t + h->field[HTTP_FIELD_HOST].sz;
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_URL],
            t,
            (int)(h->field[HTTP_FIELD_URL].str - (t - h->field[HTTP_FIELD_URL].str - 1))
        );
    }
    /* Set limit search */
    h->limit   = (
        ((h->field[HTTP_FIELD_URL].str == NULL) ||
            ((h->field[HTTP_FIELD_URL].str[0] == '/') &&
                (h->field[HTTP_FIELD_URL].str[1] == '\0'))) ? 2 : 3
    );

#   if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
    __MACRO_TOSCREEN__("\t--- [%s]\t[%s]",           htoken[HTTP_FIELD_METHOD].str, hmethod[mid].str);
    __MACRO_TOSCREEN__("\t--- [%s]\t[%d]",           htoken[HTTP_FIELD_SLIMIT].str, h->limit);
    __MACRO_TOSCREEN__("\t--- [%s]\t\t[%s] [%d]",    htoken[HTTP_FIELD_HOST].str,   h->field[HTTP_FIELD_HOST].str, h->field[HTTP_FIELD_HOST].sz);
    __MACRO_TOSCREEN__("\t--- [%s]\t\t[%s] [%d]",    htoken[HTTP_FIELD_PORT].str,   h->field[HTTP_FIELD_PORT].str, h->field[HTTP_FIELD_PORT].sz);
 /* __MACRO_TOSCREEN__("\t--- [%s]\t\t[%s] [%d]",    htoken[HTTP_FIELD_ORIGIN].str, h->origin[HTTP_FIELD_ORIGIN].str, h->origin[HTTP_FIELD_ORIGIN].sz); */
    __MACRO_TOSCREEN__("\t--- [%s]\t\t[%s] [%d]",    htoken[HTTP_FIELD_URL].str,    h->field[HTTP_FIELD_URL].str, h->field[HTTP_FIELD_URL].sz);
    __MACRO_TOSCREEN__("\t--- [Source]:\n\t[\n\t\t%s\n\t]\n", cc);
#   endif

#   if defined(HAVE_HTTP_ALLOW_CONNECT)
    return ((h->field[HTTP_FIELD_HOST].str == NULL) ?
        HTTP_HEADER_ERROR :
            ((mid == HTTP_METHOD_CONNECT) ? HTTP_PARSE_CHANGE_DST : HTTP_PARSE_OK)
    );
#   else
    return ((h->field[HTTP_FIELD_HOST].str == NULL) ? HTTP_HEADER_ERROR : HTTP_PARSE_OK);
#   endif
}

/* HTTPS Connection if SNI extension present */

static int __parse_sni(unsigned char *bytes, string_s *s)
{
    /*
       1,2  tls version 1.0
       9,10 tls version 1.1
     */
    if (
        (bytes[1] != 0x03) || (bytes[2]  != 0x01) ||
        (bytes[9] != 0x03) || (bytes[10] != 0x03)
       )
    {
        return 0;
    }

    unsigned char  *cpos, *maxchar, cmplen, offlen = bytes[43];
    unsigned short  clen, etype = 1, elen, nlen;

    cpos    = bytes + 1 + 43 + offlen;
    clen    = ntohs(*(unsigned short*)cpos);            cpos += 2 + clen;
    cmplen  = *cpos;                                    cpos += 1 + cmplen;
    maxchar = cpos + 2 + ntohs(*(unsigned short*)cpos); cpos += 2;

    while((cpos < maxchar) && (etype != 0))
    {
        etype = ntohs(*(unsigned short*)cpos); cpos += 2;
        elen  = ntohs(*(unsigned short*)cpos); cpos += 2;
        if(etype == 0)
        {
            cpos += 3;
            nlen  = ntohs(*(unsigned short*)cpos);
            cpos += 2;
#           if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
            __MACRO_TOSCREEN_URL__("", "found SSL-SNI extension: %s [%d]", 's', (char*)cpos, *len);
#           endif
            s->str = (char*)cpos;
            s->sz  = nlen;
            return 1;
        }
        else
        {
            cpos += elen;
        }
    }
#   if (defined(DEBUG_URL_PACKET) && defined(DEBUG_NODAEMON))
    if (cpos != maxchar)
    {
        __MACRO_TOSCREEN_URL__("", "SSL header incomplete, no SNI extension", 's');
    }
#   endif
    return -1;
}
FUNEXPORT int https_parse_sni(string_s *s, http_parse_s *h)
{
    if ((s == NULL) || (s->str == NULL) || (s->sz == 0))
    {
        return HTTP_HEADER_EMPTY;
    }
    switch(__parse_sni((unsigned char*)s->str, &h->field[HTTP_FIELD_HOST]))
    {
        case -1:
        {
            return HTTP_HEADER_ERROR;
        }
        case 0:
        {
            return HTTP_HEADER_NOTIMPL;
        }
        default:
        {
            __HTTP_FIELD_ADD(
                h->field[HTTP_FIELD_PORT],
                CP HTTPS_PORT_STRING,
                __CSZ(HTTPS_PORT_STRING)
            );
            h->limit = 2;
            return HTTP_PARSE_OK;
        }
    }
}

FUNEXPORT int https_parse_dst(sa_socket_t *sa, string_s *s, http_parse_s *h, int *err)
{
    *err = 0;
#   if defined(SO_ORIGINAL_DST)
    do
    {
        int   csz;
        char *ctmp;
        memset(s->str, '\0', s->sz);

        if (inet_ntop(AF_INET, &(sa->sa.sin_addr), s->str, INET_ADDRSTRLEN) == NULL)
        {
            *err = 9;
            break;
        }
        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_HOST],
            s->str,
            0
        );

        ctmp = (s->str + h->field[HTTP_FIELD_HOST].sz + 1);
        csz  = string_itoa(ntohs(sa->sa.sin_port), ctmp);

        __HTTP_FIELD_ADD(
            h->field[HTTP_FIELD_PORT],
            ctmp,
            csz
        );

        h->limit = 2;
        return HTTP_PARSE_OK;

    } while(0);

#   endif

    return HTTP_HEADER_ERROR;
}
