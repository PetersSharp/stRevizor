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

#ifndef REVIZOR_XML_PARSER_H
#define REVIZOR_XML_PARSER_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if defined(HAVE_ARPA_INET_H)
#  include <arpa/inet.h>
#else
#  error "Needed source header: arpa/inet.h"
#endif

#if defined(HAVE_NETINET_IN_H)
#  include <netinet/in.h>
#else
#  error "Needed source header: netinet/in.h"
#endif

#if defined(HAVE_NETDB_H)
#  include <netdb.h>
#else
#  error "Needed source header: netdb.h"
#endif

#include "../st-revizor-lock.h"

#define __LOOP_FORMAT_I  "%c|%s\n"
#define __LOOP_FORMAT_D  "%c|%s\n"
#define __LOOP_FORMAT_DM "%c|.%s\n"
#define __LOOP_FORMAT_B  "%c|%s/%d\n"
#define __LOOP_FORMAT_BF "%c|%s/32\n"
#define __LOOP_FORMAT_UI "%c|%s|%s\n"
#define __LOOP_FORMAT_U  "%c|%s|%s|/%s\n"

#define __XML_ELE_TOKEN  "</content>"
#define __SQ_TOKEN_HTTP  "http://"
#define __SQ_TOKEN_HTTPS "https://"
#define __SQ_TOKEN_EMPTY ""
#define __SQ_TOKEN_ALL   "*"

#ifdef XML_LARGE_SIZE
#  if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#    define XML_FMT_INT_MOD "I64"
#  else
#    define XML_FMT_INT_MOD "ll"
#  endif
#else
#  define XML_FMT_INT_MOD "l"
#endif

#define __LOOP_WDATA(A,B,C,...) \
    { \
        char *ptr = (C); \
        do { \
            if (!*ptr) { break; } \
            fprintf((A), (B), *ptr, __VA_ARGS__); \
        } while (*ptr++); }

#define INIT_NTOP() \
    char ntop_ips[INET_ADDRSTRLEN]; \
    struct in_addr ntop_addr

#define SET_NTOP(A,B) \
    { \
        unsigned long uaddr = (unsigned long)(A); \
        if (uaddr > 0UL) { \
            memcpy((void*)&ntop_addr.s_addr, (void*)&uaddr, sizeof(unsigned long)); \
        } \
        memset((void*)&ntop_ips, '\0', INET_ADDRSTRLEN); \
        if (inet_ntop(AF_INET, &(ntop_addr), ntop_ips, INET_ADDRSTRLEN) != NULL) { \
            B; \
        } \
    }

#define __PTH_PARSER_EXIT(A,B,C,D,E) \
    { \
        if (A != NULL) { fclose(A); } \
        if (B != NULL) { iconv(B, NULL, NULL, NULL, NULL); iconv_close(B); } \
        if (C != NULL) { XML_ParserFree(C); } \
        free_xmlele_list(E); \
        pthread_exit((void*)D); \
    }

#define __ARR_TMP_INIT(A) \
    ip_to_arr_t __EV(l,A); \
    __cleanup_set_free(ip_to_arr_t,__EV(p,A),&(__EV(l,A))); \
    memset(&(__EV(l,A)), 0, sizeof(ip_to_arr_t))

typedef unsigned long (*ip_proc_fun)(unsigned long, void*);

typedef enum __xml_ele_e_
{
    XML_ELE_IP,
    XML_ELE_DNS,
    XML_ELE_URL,
    XML_ELE_WHITE,
    XML_ELE_CNT,
    XML_ELE_REG,
    XML_ELE_NONE
} xml_ele_e;

typedef enum __xml_attr_e_
{
    XML_ATTR_IP,
    XML_ATTR_DNS,
    XML_ATTR_DNSM,
    XML_ATTR_DEF
} xml_attr_e;

typedef struct __ip_to_arr_t_
{
    st_table_lock_t lock;
    unsigned long   idx;
    unsigned long  *arr;
} ip_to_arr_t;

typedef struct __xml_ele_t_
{
    FILE       *fp, *squhttp, *squhttps, *sqphttp, *sqphttps;
    xml_ele_e   type;
    xml_attr_e  attr;
    string_s  **ip;
    string_s  **dns;
    string_s  **url;
    int         cip, cdns, curl, cport;
    ip_to_arr_t lip_ip;
    ip_to_arr_t lip_dns;
    ip_to_arr_t lip_url;
    ip_to_arr_t lip_bgp;
    ip_to_arr_t lip_phttp;
    ip_to_arr_t lip_phttps;
    ip_to_arr_t lstr_uhttps;
    ip_to_arr_t lstr_dns;
    ip_to_arr_t lstr_url;
    ip_to_arr_t lstr_host;
} xml_ele_t;

typedef struct __xml_search_t_
{
    xml_ele_e   type;
    string_s    token;
} xml_search_t;

typedef struct __xml_uri_parse_t_ {
    int      ishttps, isport;
    string_s wildcard;
    string_s regpath;
    string_s port;
    string_s host;
    string_s path;
} xml_uri_t;

typedef struct __xml_path_parse_t_ {
    string_s *spath;
    string_s *opath;
} xml_path_t;

// Extern, from st-revizor-hash.c
unsigned long hash_string(string_s*);

void          plist_data(xml_ele_t*);
void          plist_from_ip(xml_ele_t*, char*);
void          plist_from_ip_round(xml_ele_t*, char*, int);
void          plist_from_dns(xml_ele_t*);
void          plist_from_url(xml_ele_t*);
void          plist_add_dns(xml_ele_t*, string_s*);
void          plist_add_url(xml_ele_t*, xml_uri_t *uri);
void          plist_from_domain(xml_ele_t*, char*);
void          plist_result_prn(xml_ele_t*);
void          plist_white_list(xml_ele_t*);
void          plist_fixed_list(xml_ele_t*);
void          plist_squid_port(xml_ele_t*);

int           plist_zip_prog(string_s*, const string_s*, const string_s*, const string_s*);
int           plist_xml_prog(string_s*, const string_s*, const string_s*);

void          arr_item_free(ip_to_arr_t*);
unsigned long arr_ip_add(ip_to_arr_t*,  string_s*, char*);
unsigned long arr_str_add(ip_to_arr_t*, string_s*, char*);
unsigned long arr_num_add(ip_to_arr_t*, unsigned long, char*);
unsigned long arr_net_add(ip_to_arr_t*, string_s*, int, char*);
int           arr_dns_add(ip_to_arr_t*, string_s*, ip_to_arr_t*, char*);

unsigned long arr_item_search(ip_to_arr_t*, unsigned long);
void          arr_item_sort(ip_to_arr_t*);

int           uri_parse(xml_uri_t*, string_s*);
char        * uri_add_regexp(xml_uri_t*);
void          uri_free_regexp(xml_uri_t*);
unsigned long uri_port(string_s*);

FUNINLINE void __EV(auto_free,ip_to_arr_t)(ip_to_arr_t **x) {
    if ((x) && (*x)) { arr_item_free(*(ip_to_arr_t**)x); }
}

#endif
