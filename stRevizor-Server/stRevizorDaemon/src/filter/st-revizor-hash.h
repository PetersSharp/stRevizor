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

#ifndef REVIZOR_HASH_H
#define REVIZOR_HASH_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include "../st-revizor-watch.h"
#include "st-revizor-lock.h"

#if !defined(__GNUC__) && !defined(__GCC__)
#   define __MACRO_CLEAR_HASHTABLE__(A,B) \
        { if (A > -1)  { __cleanup_notsupport(fclose, A); } \
        if (B != NULL) { __cleanup_notsupport(hash_table_free,&(B)); }}
#else
#   define __MACRO_CLEAR_HASHTABLE__(A,B)
#endif

#if !defined(__GNUC__) && !defined(__GCC__)
#   define __MACRO_CLEAR_HASHTREE__(A,B,C) \
        { if (A > -1)  { __cleanup_notsupport(fclose,A); } \
        if (B != NULL) { __cleanup_notsupport(hash_tree_free,&(B)); } \
        if (C != NULL) { __cleanup_notsupport(auto_freechar, &(C)); }}
#else
#   define __MACRO_CLEAR_HASHTREE__(A,B,C)
#endif

typedef void (*hash_portlist_cb)(void*, void*);

typedef enum __hash_tree_type__ {
    HASH_TREE_ELE_MARK =  0,
    HASH_TREE_ELE_PORT,
    HASH_TREE_ELE_HOST,
    HASH_TREE_ELE_URL
} hash_tree_type_e;

typedef enum __hash_tree_response__ {
    HASH_TREE_NOTFOUND =  0,
    HASH_TREE_FOUND    =  1,
    HASH_TREE_ERRQUERY = -1
} hash_tree_response_e;

typedef struct __hash_custom_ele__
{
    int           cnt, len;
    unsigned long hash;
} hash_ele_t;

typedef struct __hash_table__
{
    unsigned long  arridx, eleidx;
    unsigned long *arr;
    hash_ele_t   **ele;
    int            elemin, elemax;
} hash_table_t;

typedef struct __hash_tree_ele__
{
    unsigned long              hash;
    unsigned long              treeidx;
    struct __hash_tree_ele__ **tree;
} hash_tree_ele_t;

typedef struct __hash_tree__
{
    unsigned long              eleidx[3];
#   if (defined(HAVE_BUILD_USE_QSORT) && (HAVE_BUILD_USE_QSORT == 1))
    hash_tree_ele_t            base;
#   else
    unsigned long              treeidx;
    hash_tree_ele_t          **tree;
#   endif
} hash_tree_t;

typedef struct __hash_tree_seach_s__
{
    int       limit;
    string_s  search[3];
} hash_tree_seach_s;

unsigned long  hash_string(string_s*);

int            hash_table_check(hash_table_t*);
void           hash_table_free(hash_table_t*);
hash_table_t * hash_table_dns_read(string_s*);
hash_tree_response_e hash_table_dns_search(hash_table_t*, char*);

int            hash_tree_check(hash_tree_t*);
void           hash_tree_free(hash_tree_t*);
hash_tree_t  * hash_tree_url_read(string_s*, void*, hash_portlist_cb);
hash_tree_response_e hash_tree_url_search(hash_tree_t*, void*, int);

FUNINLINE void __EV(auto_free, hash_table_t)(hash_table_t **x) {
    if ((x) || (*x)) { hash_table_free(*(hash_table_t**)x); x = (void*)0; }
}
FUNINLINE void __EV(auto_free, hash_tree_t)(hash_tree_t **x) {
    if ((x) || (*x)) { hash_tree_free(*(hash_tree_t**)x); x = (void*)0; }
}

#endif
