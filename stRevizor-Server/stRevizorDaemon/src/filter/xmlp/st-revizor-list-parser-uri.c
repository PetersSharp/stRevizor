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

#define __URI_FIELD_ADD(A,B,C) \
    A.sz  = C; \
    A.sz  = ((A.sz <= 0) ? strlen(B) : A.sz); \
    A.str = (B)

string_s uriscm[] = {
    { __ADD_string_s("http://") },
    { __ADD_string_s("https://") }
};

string_s uridefport[] = {
    { __ADD_string_s("80") },
    { __ADD_string_s("443") }
};

const char urireg[] = {
    '{',
    '[',
    '}',
    ']'
};

int uri_parse(xml_uri_t *uri, string_s *url)
{
    if ((url == NULL) || (url->str == NULL) || (url->sz == 0))
    {
        return -1;
    }
    int  i, off = 0;
    char *t = NULL, *cc = NULL, *c = (char*)url->str;
    uri->path.str = NULL; uri->path.sz = uri->isport = 0;

    for (i = 0; i < __NELE(uriscm); i++)
    {
        if (memcmp((void*)c, (void*)uriscm[i].str, uriscm[i].sz) == 0)
        {
            off = uriscm[i].sz;
            uri->ishttps = i;
            break;
        }
    }
    if (off == 0)
    {
        return -1;
    }

    c = cc = (c + off);

    if ((t = strsep((char**)&cc, "/")) != NULL)
    {
        __URI_FIELD_ADD(
            uri->host,
            t,
            0
        );
        if (cc != NULL)
        {
            __URI_FIELD_ADD(
                uri->path,
                c + (cc - t),
                0
            );
        }
        c = uri->host.str;
    }
    else
    {
        __URI_FIELD_ADD(
            uri->host,
            c,
            0
        );
    }
    if (c == NULL)
    {
        return -1;
    }
    if ((t = strchr(c, ':')) != NULL)
    {
        if ((t = strsep((char**)&c, ":")) != NULL)
        {
            __URI_FIELD_ADD(
                uri->host,
                t,
                0
            );
            if ((c != NULL) && ((t = strsep((char**)&c, ":")) != NULL))
            {
                unsigned long p = strtoul(t, &c, 10);
                if ((p > 0UL) && (p <= 65535UL))
                {
                    __URI_FIELD_ADD(
                        uri->port,
                        t,
                        0
                    );
                }
            }
            else
            {
                return -1;
            }
        }
    }
    if (uri->port.sz == 0)
    {
        __URI_FIELD_ADD(
            uri->port,
            uridefport[uri->ishttps].str,
            uridefport[uri->ishttps].sz
        );
        uri->isport = 1;
    }

    // Normalize URI path.. TODO..
    uri->path.str = ((uri->path.sz  == 0) ? NULL : uri->path.str);
    uri->path.sz  = ((uri->path.str == NULL) ? 0 : uri->path.sz);

    if (
        (uri->path.str == NULL) ||
        (uri->path.str[(uri->path.sz - 1)] == '/')
       )
    {
        __SET_string_size_s(uri->wildcard, __SQ_TOKEN_ALL);
    }
    else
    {
        __SET_string_size_s(uri->wildcard, __SQ_TOKEN_EMPTY);
    }

    /*
        // DEBUG

        printf(
            "\tURI: OK\tis https ?(%d)\n\t\tport:\t%s [%d]\n\t\thost:\t%s [%d]\n\t\tpath:\t%s [%d]\n\t\twildcard:\t%s [%d]\n\n",
            uri->ishttps, uri->port.str, uri->port.sz, uri->host.str, uri->host.sz, uri->path.str, uri->path.sz,
            uri->wildcard.str, uri->wildcard.sz
        );
    */

    return 0;
}

unsigned long uri_port(string_s *url)
{
    if ((url == NULL) || (url->str == NULL) || (url->sz == 0))
    {
        return 0UL;
    }
    unsigned long i = 0, sz, ret = 0UL;
    char tstr[20], *t, *c = url->str;

    while ((t = strchr(c, ':')) != NULL)
    {
        t  = t + 1;
        sz = (unsigned long)(t - c);
        c  = t;

        if (c[0] == '/')
        {
            continue;
        }

        memset(&tstr, '\0', 20);

        for (i = 0; ((i < sz) || (i < 20)); i++)
        {
            if ((t[i] < '0') || (t[i] > '9')) { break; }
            tstr[i] = t[i];
        }
        if (i == 0)
        {
            continue;
        }
        if ((ret = strtoul(tstr, &t, 10)) > 0UL)
        {
            return ((ret > 65535UL) ? 0UL : ret);
        }
    }
    return 0;
}

static void __uri_free_regexp(xml_uri_t *uri)
{
    if (uri->regpath.str != NULL)
    {
        free(uri->regpath.str);
        uri->regpath.str = NULL;
        uri->regpath.sz  = 0;
    }
}

static int __uri_regexp_alloc(xml_uri_t *uri, char *src, int sz)
{
    if ((src == NULL) || (sz == 0))
    {
        return 0;
    }

    __uri_free_regexp(uri);

    if ((uri->regpath.str = calloc(1, (sz + 1))) == NULL)
    {
        return 0;
    }
    memcpy((void*)uri->regpath.str, (void*)src, sz);
    uri->regpath.sz = sz;
    uri->regpath.str[uri->regpath.sz] = '\0';
    return 1;
}

void uri_free_regexp(xml_uri_t *uri)
{
    __uri_free_regexp(uri);
}

char * uri_add_regexp(xml_uri_t *uri)
{
    if (
        (uri == NULL) ||
        ((uri->path.str == NULL) || (uri->path.sz == 0))
       )
    {
        return CP __SQ_TOKEN_EMPTY;
    }

    __uri_free_regexp(uri);

    int i, ismod = 0, idx = 0;
    char rbuf[(uri->path.sz + 3)], *t, *c = CP rbuf;
    memcpy((void*)&rbuf, (void*)uri->path.str, uri->path.sz);

    for (i = 0; i < __NELE(urireg); i++)
    {
        if ((t = strchr(c, urireg[i])) != NULL)
        {
            ismod    = 1;
            idx      = (int)(t - c);
            c[idx++] = '.';
            c[idx++] = '*';
            c[idx]   = '\0';
        }
    }
    if (ismod)
    {
        __SET_string_size_s(uri->wildcard, __SQ_TOKEN_EMPTY);
        ismod = __uri_regexp_alloc(uri, c, idx);
    }
    return ((ismod) ? uri->regpath.str : uri->path.str);
}
