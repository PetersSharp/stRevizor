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
#include "st-revizor-xml-parser.h"

void plist_add_dns(xml_ele_t *ele, string_s *dns)
{
    char *f;
    __BUILD_string_s(src, dns->str);
    xml_attr_e attr = (((*(src.str) == '*') || (*(src.str) == '.')) ? XML_ATTR_DNSM : ele->attr);

    switch(attr)
    {
#       if 1
        case XML_ATTR_IP:
#       endif
#       if 1
        case XML_ATTR_DEF:
#       endif
        case XML_ATTR_DNS:
        {
            f = CP __LOOP_FORMAT_D;
            break;
        }
        case XML_ATTR_DNSM:
        {
            if (*(src.str) == '*' ) { src.str++; }
            if (*(src.str) == '.' ) { f = CP __LOOP_FORMAT_D;  }
            else                    { f = CP __LOOP_FORMAT_DM; }
            break;
        }
        default:
        {
            return;
        }
    }

    src.sz = strlen(src.str);

    if (arr_str_add(&ele->lstr_dns, &src, CP "DNS BlackList") == 0UL)
    {
        return;
    }
    __LOOP_WDATA(ele->fp, f, CP "D", src.str);
}

void plist_from_dns(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }
    for (int i = 0; i < ele->cdns; i++)
    {
        plist_add_dns(ele, ele->dns[i]);
    }
}
void plist_from_domain(xml_ele_t *ele, char *mark)
{
    if (ele == NULL)
    {
        return;
    }

    int i, n;
    INIT_NTOP();
    __ARR_TMP_INIT(tmp);

    for (i = 0; i < ele->cdns; i++)
    {
        if ((ele->dns[i]->str == NULL) || (ele->dns[i]->sz <= 0))
        {
            continue;
        }

        string_s tmp = { ele->dns[i]->str, ele->dns[i]->sz };

        if ((*tmp.str == '*') || (*tmp.str == '.'))
        {
            tmp.str++; tmp.sz--;

            if (*tmp.str == '.')
            {
                tmp.str++; tmp.sz--;
            }
        }
        if (arr_dns_add(&ele->lip_ip, &tmp, ptmp, CP "IP from DNS BlackList") == 0)
        {
            for (n = 0; n < ptmp->idx; n++)
            {
                SET_NTOP(
                    ptmp->arr[n],
                    __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I, mark, ntop_ips)
                );
            }
            __cleanup_notsupport_p(auto_freeip_to_arr_t, ptmp);
        }
    }
}
void plist_from_ip(xml_ele_t *ele, char *mark)
{
    if (ele == NULL)
    {
        return;
    }

    int i;

    for (i = 0; i < ele->cip; i++)
    {
        if (arr_ip_add(&ele->lip_ip, ele->ip[i], CP "IP BlackList") == 0UL)
        {
            continue;
        }
        __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I, mark, ele->ip[i]->str);
    }
}
void plist_from_ip_round(xml_ele_t *ele, char *mark, int mask)
{
    if (ele == NULL)
    {
        return;
    }

    INIT_NTOP();

    for (int i = 0; i < ele->cip; i++)
    {
        if ((ntop_addr.s_addr = arr_net_add(&ele->lip_bgp, ele->ip[i], mask, CP "NET round BlackList")) == 0UL)
        {
            continue;
        }
        SET_NTOP(
            0UL,
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_B, mark, ntop_ips, mask)
        );
    }
}
void plist_add_url(xml_ele_t *ele, xml_uri_t *uri)
{
    if (ele == NULL)
    {
        return;
    }

    int i;
    __ARR_TMP_INIT(tmp);

    if (uri->ishttps)
    {
        for (i = 0; i < ele->cip; i++)
        {
            if (arr_ip_add(&ele->lip_url, ele->ip[i], CP "URL BlackList (https from ip)") == 0UL)
            {
                continue;
            }
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_UI, "U", uri->port.str, ele->ip[i]->str);
        }

#       if (defined(HAVE_XML_PARSER_RESOLVE_IP_HTTPS) && (HAVE_XML_PARSER_RESOLVE_IP_HTTPS == 1))
        INIT_NTOP();

        if (arr_dns_add(&ele->lip_url, &uri->host, ptmp, CP "URL BlackList (https from dns)") == 0UL)
        {
            for (i = 0; i < ptmp->idx; i++)
            {
                SET_NTOP(
                    ptmp->arr[i],
                    __LOOP_WDATA(ele->fp, __LOOP_FORMAT_UI, CP "U", uri->port.str, ntop_ips)
                );
            }

#           if defined(HAVE_HTTPS_SNI_RESOLV)
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_UI, CP "U", uri->port.str, uri->host.str)
#           endif

            __cleanup_notsupport_p(auto_freeip_to_arr_t, ptmp);
        }
#       endif

#       if (defined(HAVE_HTTPS_BACK_RESOLV) && (HAVE_HTTPS_BACK_RESOLV == 1))
        __BUFFER_CREATE_SIZE__(hres, 1024);
        sa_socket_t sas = {0};
        for (i = 0; i < ele->cip; i++)
        {
            if (net_default_sa_socket_set(&sas, ele->ip[i], "HTTPS resolv back DNS") == 0)
            {
                if (net_sa_to_dns((sa_socket_t*)&sas, (string_s*)&hres) == 0)
                {
                    if (arr_str_add(&ele->lstr_host, &hres, CP "URL reverse BlackList (https ip to dns)") == 0UL)
                    {
                        continue;
                    }
                    __LOOP_WDATA(ele->fp, __LOOP_FORMAT_UI, CP "U", uri->port.str, hres.str);
                }
            }
        }
#       endif
    }
    else
    {
        if (
            (uri->path.str != NULL) &&
            ((uri->path.sz > 1) || ((uri->path.sz == 1) && (uri->path.str[0] != '/')))
           )
        {
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_U, CP "U", uri->port.str, uri->host.str, uri->path.str);
        }
        else
        {
            if (arr_str_add(&ele->lstr_host, &uri->host, CP "URL BlackList (http)") == 0UL)
            {
                return;
            }
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_UI, CP "U", uri->port.str, uri->host.str);
        }
    }
}
void plist_add_squid(xml_ele_t *ele, xml_uri_t *uri)
{
    if (ele == NULL)
    {
        return;
    }

    unsigned long port = strtoul(uri->port.str, NULL, 10);
    FILE *fp = ((uri->ishttps) ?  ele->squhttps : ele->squhttp);

    if (fp != NULL)
    {
        switch (uri->ishttps)
        {
            case 0:
            {
                char *upath = uri_add_regexp(uri);

                if ((!uri->isport) && ((port > 0UL) && (port >= 65535UL)))
                {
                    fprintf(fp, "%s:%lu/%s%s\n", uri->host.str, port, upath, uri->wildcard.str);
                }
                else
                {
                    fprintf(fp, "%s/%s%s\n", uri->host.str, upath, uri->wildcard.str);
                }
                uri_free_regexp(uri);
                break;
            }
            case 1:
            {
                if (arr_str_add(&ele->lstr_uhttps, &uri->host, CP "HTTPS Host BlackList") > 0UL)
                {
                    fprintf(fp, "%s:%s\n", uri->host.str, uri->port.str);
                }
                break;
            }
        }
    }

    fp = ((uri->ishttps) ? ele->sqphttps : ele->sqphttp);

    if (fp != NULL)
    {
        if ((port > 0UL) && (port <= 65535UL))
        {
            if (
                arr_num_add(
                    ((uri->ishttps) ? &ele->lip_phttps : &ele->lip_phttp),
                    port,
                    CP "URL Squid (url port)"
                ) > 0UL)
            {
                ele->cport++;
            }
        }
    }
}

void plist_from_url(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }
    for (int i = 0; i < ele->curl; i++)
    {
        if (arr_str_add(&ele->lstr_url, ele->url[i], CP "URL BlackList (complex url)") == 0UL)
        {
            continue;
        }

        xml_uri_t uri = {0};
        if (uri_parse(&uri, ele->url[i]) == 0)
        {
#           if (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1))
            plist_add_url(ele, &uri);
#           endif
#           if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
            plist_add_squid(ele, &uri);
#           endif
        }
    }
}

void plist_squid_port(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }

    int i;

    if (ele->sqphttp != NULL)
    {
        for (i = 0; i < ele->lip_phttp.idx; i++)
        {
            fprintf(ele->sqphttp, "%lu\n", ele->lip_phttp.arr[i]);
        }
    }
    if (ele->sqphttps != NULL)
    {
        for (i = 0; i < ele->lip_phttps.idx; i++)
        {
            fprintf(ele->sqphttps, "%lu\n", ele->lip_phttps.arr[i]);
        }
    }
}
void plist_data(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }

    /* url filter enable */
#   if ((defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)) || \
        (!defined(HAVE_XML_PARSER_RESOLVE_IP_ALL) || (HAVE_XML_PARSER_RESOLVE_IP_ALL == 0)))
    if (ele->curl == 0)
    {
        plist_from_ip(ele, CP "R");
    }
#   endif

    /* url or squid filter enable */
#   if ((defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1)) || \
        (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)))
        plist_from_url(ele);
#   endif

    /* dns filter enable */
#   if (defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1))
    plist_from_dns(ele);
#   endif

    /* dns && url filters disabled */
#   if ((!defined(HAVE_DNS_FILTER_ENABLE)  || (HAVE_DNS_FILTER_ENABLE == 0)) && \
        (!defined(HAVE_HTTP_FILTER_ENABLE) || (HAVE_HTTP_FILTER_ENABLE == 0)))
#     if (defined(HAVE_XML_PARSER_RESOLVE_IP_ALL) && (HAVE_XML_PARSER_RESOLVE_IP_ALL == 1))
        plist_from_domain(ele, CP "R");
#     endif
#     if (defined(HAVE_XML_PARSER_BLOCK_IP_ALL) && (HAVE_XML_PARSER_BLOCK_IP_ALL == 1))
        plist_from_ip(ele, CP "R");
#     endif
#   endif

    plist_from_ip_round(ele, CP "B", 16);
}
