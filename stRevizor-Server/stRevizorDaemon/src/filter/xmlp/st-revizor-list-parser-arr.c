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

#define __ARR_ALLOC_ITEMS 1000

#if (defined(HAVE_BUILD_USE_QSORT) && (HAVE_BUILD_USE_QSORT == 1))
static int arr_cmp_(const void *a, const void *b)
{
    if ( *(unsigned long*)a <  *(unsigned long*)b ) { return -1; }
    if ( *(unsigned long*)a == *(unsigned long*)b ) { return  0;  }
    if ( *(unsigned long*)a >  *(unsigned long*)b ) { return  1;  }
    return 0;
}
static void arr_item_sort_(ip_to_arr_t *arr)
{
    qsort(arr->arr, arr->idx, sizeof(unsigned long), arr_cmp_);
}
static unsigned long arr_item_search_(ip_to_arr_t *arr, unsigned long item)
{
    if ((arr == NULL) || (arr->idx == 0UL)) { return 0UL; }
    unsigned long lout, *data;

    st_read_lock(&arr->lock);
    if ((data = bsearch(
        (const void*)&item,
        (const void*)arr->arr,
        arr->idx,
        sizeof(unsigned long),
        arr_cmp_)
    ) == NULL) { lout = 0UL; }
    else       { lout = *(data); }
    st_read_unlock(&arr->lock);
    return lout;
}
#else
/*
    compatible warning: another sort order, not compatible qsort,bsearch
    very slow methods :) TODO: use MMX,SSE
*/
static void arr_item_sort_(ip_to_arr_t *arr)
{
    unsigned long i, low, val1, val2, *arr0 = arr->arr;
    for (low = 0; low < (arr->idx - 1); low++)
    {
        for (i = 0; i < (arr->idx - low - 1); i++)
        {
            __builtin_prefetch(&arr0[(i+1)]);
            if (arr0[i] > arr0[(i+1)])
            {
                val1        = arr0[i];
                val2        = arr0[(i+1)];
                arr0[i]     = val2;
                arr0[(i+1)] = val1;
            }
        }
    }
}
static unsigned long arr_item_search_(ip_to_arr_t *arr, unsigned long ip)
{
    if ((arr == NULL) || (arr->idx == 0UL)) { return 0UL; }
    st_read_lock(&arr->lock);
    unsigned long hi = arr->idx, low = 0UL;
    __builtin_prefetch(arr->arr + (arr->idx * sizeof(unsigned long)), 0,1,1);
    while (low < hi)
    {
        unsigned long mid = ((hi + low) / 2);
        if (!arr->arr[mid])           { continue; }
        else if (arr->arr[mid] == ip) { st_read_unlock(&arr->lock); return arr->arr[mid]; }
        else if (arr->arr[mid] >  ip) { hi  = mid; }
        else                          { low = (mid + 1); }
    }
    st_read_unlock(&arr->lock);
    return 0UL;
}
#endif

unsigned long arr_item_search(ip_to_arr_t *arr, unsigned long item)
{
    return arr_item_search_(arr, item);
}
void arr_item_sort(ip_to_arr_t *arr)
{
    arr_item_sort_(arr);
}

static size_t arr_item_alloc(ip_to_arr_t *arr, unsigned long item, char *stype)
{
    do {
        if ((arr == NULL) || (item == 0UL)) { break; }
        st_write_lock(&arr->lock);
        if (arr->arr == NULL)
        {
            if ((arr->arr = calloc((__ARR_ALLOC_ITEMS + 1), sizeof(unsigned long))) == NULL)
            {
                break;
            }
            arr->idx = 1;
        }
        else if ((arr->idx % 1000) == 0)
        {
            if ((arr->arr = realloc(arr->arr, ((++(arr->idx) + __ARR_ALLOC_ITEMS + 1) * sizeof(unsigned long)))) == NULL)
            {
                break;
            }
            __MACRO_TOSCREEN__(STR_XML_INFO "-> XML complette parse %s items: %lu", stype, arr->idx);
        }
        else
        {
            arr->idx++;
        }
        memcpy((void*)&arr->arr[(arr->idx - 1)], (void*)&item, sizeof(unsigned long));
        if (arr->idx > 1)
        {
            arr_item_sort_(arr);
        }
        st_write_unlock(&arr->lock);
        return (size_t)(arr->idx - 1);

    } while(0);

    st_write_unlock(&arr->lock);
    return -1;
}
static unsigned long arr_ip_to_ul_(string_s *src)
{
    struct in_addr ia = {0};
    __cleanup_new_free(char) t = NULL;

    if ((src == NULL) || (src->str == NULL))
    {
        return 0UL;
    }
    if ((t = strchr(src->str, '/')) != NULL)
    {
        int sz = (int)(t - src->str);
        t = calloc(1, (sz + 1));
        memcpy((void*)t, (void*)src->str, sz);
    }
    if (inet_pton(AF_INET, ((t == NULL) ? src->str : t), &(ia)) == -1)
    {
#       if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
        __MACRO_TOSYSLOG__(LOG_ERR, STR_XML_ERROR "convet IP '%s' | '%s' error -> %s", src->str, ((t == NULL) ? CP "*" : t), strerror(errno));
#       endif
        __cleanup_notsupport_p(auto_freechar, t);
        return 0UL;
    }
#   if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))
    __MACRO_TOSCREEN__(STR_XML_INFO "convet IP '%s' | '%s' -> %lu", src->str, ((t == NULL) ? CP "*" : t), (unsigned long)ia.s_addr);
#   endif
    __cleanup_notsupport_p(auto_freechar, t);

    return (unsigned long)ia.s_addr;
}
static unsigned long arr_str_to_ul_(string_s *src)
{
    unsigned long hash;

    if ((src == NULL) || (src->str == NULL))
    {
        return 0UL;
    }
    if ((hash = hash_string(src)) == 0UL)
    {
#       if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
        __MACRO_TOSYSLOG__(LOG_ERR, STR_XML_ERROR "convet string '%s' error -> %s", src->str, strerror(errno));
#       endif
        return 0UL;
    }
#   if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))
    __MACRO_TOSCREEN__(STR_XML_INFO "convet string '%s' -> %lu", src->str, (unsigned long)hash);
#   endif

    return hash;
}
static unsigned long arr_item_add_(ip_to_arr_t *arr, unsigned long item, ip_proc_fun fun, void *data, char *stype)
{
    if ((arr == NULL) || (item == 0UL))
    {
        return 0UL;
    }
    if (fun != NULL)
    {
        if ((item = fun(item, data)) == 0UL)
        {
            return 0UL;
        }
    }
    if (arr_item_search_(arr, item) != 0UL)
    {
#       if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))
        __MACRO_TOSCREEN__(STR_XML_INFO "-> XML ITEM duplicate %s: '%lu'", stype, item);
#       endif
        return 0UL;
    }
    if (arr_item_alloc(arr, item, stype) == -1)
    {
#       if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
        __MACRO_TOSYSLOG__(LOG_ERR, STR_XML_ERROR "-> ITEM '%lu', memory error: %s", item, strerror(errno));
#       endif
        return 0UL;
    }
#   if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))
    __MACRO_TOSCREEN__("\t\t-> XML ITEM save %s: '%lu'", stype, item);
#   endif

    return item;
}
static unsigned long __ip_net_fun(unsigned long ip, void *data)
{
    switch ((int)((data == NULL) ? 32 : *((int*)data)))
    {
        case 24:
        {
            ip = ip & 0xFFFFFF;
            break;
        }
        case 16:
        {
            ip = ip & 0xFFFF;
            break;
        }
        case 8:
        {
            ip = ip & 0xFF;
            break;
        }
        default:
        {
            break;
        }
    }
    return ip;
}

/* Public array method */

unsigned long arr_ip_add(ip_to_arr_t *arr, string_s *src, char *stype)
{
    unsigned long item;
    if ((item = arr_ip_to_ul_(src)) == 0UL) { return 0UL; }
    return arr_item_add_(arr, item, NULL, NULL, stype);
}
unsigned long arr_net_add(ip_to_arr_t *arr, string_s *src, int mask, char *stype)
{
    unsigned long item;
    if ((item = arr_ip_to_ul_(src)) == 0UL) { return 0UL; }
    return arr_item_add_(arr, item, __ip_net_fun, (void*)&mask, stype);
}
unsigned long arr_str_add(ip_to_arr_t *arr, string_s *src, char *stype)
{
    unsigned long item;
    if ((item = arr_str_to_ul_(src)) == 0UL) { return 0UL; }
    return arr_item_add_(arr, item, NULL, NULL, stype);
}
unsigned long arr_num_add(ip_to_arr_t *arr, unsigned long item, char *stype)
{
    return arr_item_add_(arr, item, NULL, NULL, stype);
}

int arr_dns_add(ip_to_arr_t *arr, string_s *host, ip_to_arr_t *lst, char *stype)
{
    int i, ret = 0;
    size_t idx;

    struct addrinfo hints = {0}, *sinfo = NULL, *p;
    struct sockaddr_in *ipv4;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((i = getaddrinfo(host->str, NULL, &hints, &sinfo)) != 0) {
#       if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> XML convert, get DNS address: %s",
            gai_strerror(i)
        );
#       endif
        return -1;
    }
    for(p = sinfo; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            ipv4 = (struct sockaddr_in*)p->ai_addr;

            /*
                 // DEBUG manual:
                 printf("array dns add: %s\n", inet_ntoa(ipv4->sin_addr));
            */

            if (arr_item_search_(arr, (unsigned long)ipv4->sin_addr.s_addr) > 0UL)
            {
                continue;
            }
            if ((idx = arr_item_alloc(arr, (unsigned long)ipv4->sin_addr.s_addr, stype)) == -1)
            {
#               if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
                __MACRO_TOSYSLOG__(LOG_ERR, STR_XML_ERROR "-> IP array memory error: %s", strerror(errno));
#               endif
                ret = -1;
                break;
            }
            if (arr_item_alloc(lst, (unsigned long)ipv4->sin_addr.s_addr, stype) == -1)
            {
#               if (defined(HAVE_XML_PARSER_DEBUGI) && (HAVE_XML_PARSER_DEBUGI == 1))
                __MACRO_TOSYSLOG__(LOG_ERR, STR_XML_ERROR "-> IP list memory error: %s", strerror(errno));
#               endif
                ret = -1;
                break;
            }
        }
    }
    if (sinfo != NULL)
    {
        freeaddrinfo(sinfo);
    }
    return ret;
}
void arr_item_free(ip_to_arr_t *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            free(arr->arr); arr->arr = NULL;
        }
        arr->idx = 0UL;
    }
}
void plist_result_prn(xml_ele_t *ele)
{
#if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))

#define __PLIST_HELPER_FMT_H "\n\tPLIST: ele -> list %s -> %lu\n\n"
#define __PLIST_HELPER_FMT_B "\tBASE %s->IP: %lu [%s]\n"
#define __PLIST_HELPER_FMT_S "\tBASE HASH %s: %lu\n"

    int i;
    INIT_NTOP();

    printf(__PLIST_HELPER_FMT_H, "ip", ele->lip_ip.idx);
    st_read_lock(&ele->lip_ip.lock);
    for (i = 0; i < ele->lip_ip.idx; i++)
    {
        SET_NTOP(
            ele->lip_ip.arr[i],
            printf(__PLIST_HELPER_FMT_B, "IP", ele->lip_ip.arr[i], ntop_ips);
        );
    }
    st_read_unlock(&ele->lip_ip.lock);

    printf(__PLIST_HELPER_FMT_H, "ip round bgp", ele->lip_bgp.idx);
    st_read_lock(&ele->lip_bgp.lock);
    for (i = 0; i < ele->lip_bgp.idx; i++)
    {
        SET_NTOP(
            ele->lip_bgp.arr[i],
            printf(__PLIST_HELPER_FMT_B, "BGP", ele->lip_bgp.arr[i], ntop_ips);
        );
    }
    st_read_unlock(&ele->lip_bgp.lock);

    printf(__PLIST_HELPER_FMT_H, "ip url", ele->lip_url.idx);
    st_read_lock(&ele->lip_url.lock);
    for (i = 0; i < ele->lip_url.idx; i++)
    {
        SET_NTOP(
            ele->lip_url.arr[i],
            printf(__PLIST_HELPER_FMT_B, "URL", ele->lip_url.arr[i], ntop_ips);
        );
    }
    st_read_unlock(&ele->lip_url.lock);

    printf(__PLIST_HELPER_FMT_H, "ip dns", ele->lip_dns.idx);
    st_read_lock(&ele->lip_dns.lock);
    for (i = 0; i < ele->lip_dns.idx; i++)
    {
        SET_NTOP(
            ele->lip_dns.arr[i],
            printf(__PLIST_HELPER_FMT_B, "DNS", ele->lip_dns.arr[i], ntop_ips);
        );
    }
    st_read_unlock(&ele->lip_dns.lock);

    printf(__PLIST_HELPER_FMT_H, "string dns (ele->lstr_dns)", ele->lstr_dns.idx);
    st_read_lock(&ele->lstr_dns.lock);
    for (i = 0; i < ele->lstr_dns.idx; i++)
    {
        printf(__PLIST_HELPER_FMT_S, "DNS", ele->lstr_dns.arr[i]);
    }
    st_read_unlock(&ele->lstr_dns.lock);

    printf(__PLIST_HELPER_FMT_H, "string url (ele->lstr_url)", ele->lstr_url.idx);
    st_read_lock(&ele->lstr_url.lock);
    for (i = 0; i < ele->lstr_url.idx; i++)
    {
        printf(__PLIST_HELPER_FMT_S, "URL", ele->lstr_url.arr[i]);
    }
    st_read_unlock(&ele->lstr_url.lock);

    printf(__PLIST_HELPER_FMT_H, "string host (ele->lstr_host)", ele->lstr_host.idx);
    st_read_lock(&ele->lstr_host.lock);
    for (i = 0; i < ele->lstr_host.idx; i++)
    {
        printf(__PLIST_HELPER_FMT_S, "URL", ele->lstr_host.arr[i]);
    }
    st_read_unlock(&ele->lstr_host.lock);

#undef __PLIST_HELPER_FMT_H
#undef __PLIST_HELPER_FMT_B

#endif
}
