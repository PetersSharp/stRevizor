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

#include "../st-revizor-watch.h"
#include "../st-revizor-inline.h"
#include "st-revizor-hash.h"

static unsigned long __attribute__ ((aligned(4))) fast_crc[256] = {
    0UL,
    1996959894UL, 3993919788UL, 2567524794UL, 124634137UL, 1886057615UL, 3915621685UL, 2657392035UL, 249268274UL, 2044508324UL, 3772115230UL,
    2547177864UL, 162941995UL, 2125561021UL, 3887607047UL, 2428444049UL, 498536548UL, 1789927666UL, 4089016648UL, 2227061214UL, 450548861UL,
    1843258603UL, 4107580753UL, 2211677639UL, 325883990UL, 1684777152UL, 4251122042UL, 2321926636UL, 335633487UL, 1661365465UL, 4195302755UL,
    2366115317UL, 997073096UL, 1281953886UL, 3579855332UL, 2724688242UL, 1006888145UL, 1258607687UL, 3524101629UL, 2768942443UL, 901097722UL,
    1119000684UL, 3686517206UL, 2898065728UL, 853044451UL, 1172266101UL, 3705015759UL, 2882616665UL, 651767980UL, 1373503546UL, 3369554304UL,
    3218104598UL, 565507253UL, 1454621731UL, 3485111705UL, 3099436303UL, 671266974UL, 1594198024UL, 3322730930UL, 2970347812UL, 795835527UL,
    1483230225UL, 3244367275UL, 3060149565UL, 1994146192UL, 31158534UL, 2563907772UL, 4023717930UL, 1907459465UL, 112637215UL, 2680153253UL,
    3904427059UL, 2013776290UL, 251722036UL, 2517215374UL, 3775830040UL, 2137656763UL, 141376813UL, 2439277719UL, 3865271297UL, 1802195444UL,
    476864866UL, 2238001368UL, 4066508878UL, 1812370925UL, 453092731UL, 2181625025UL, 4111451223UL, 1706088902UL, 314042704UL, 2344532202UL,
    4240017532UL, 1658658271UL, 366619977UL, 2362670323UL, 4224994405UL, 1303535960UL, 984961486UL, 2747007092UL, 3569037538UL, 1256170817UL,
    1037604311UL, 2765210733UL, 3554079995UL, 1131014506UL, 879679996UL, 2909243462UL, 3663771856UL, 1141124467UL, 855842277UL, 2852801631UL,
    3708648649UL, 1342533948UL, 654459306UL, 3188396048UL, 3373015174UL, 1466479909UL, 544179635UL, 3110523913UL, 3462522015UL, 1591671054UL,
    702138776UL, 2966460450UL, 3352799412UL, 1504918807UL, 783551873UL, 3082640443UL, 3233442989UL, 3988292384UL, 2596254646UL, 62317068UL,
    1957810842UL, 3939845945UL, 2647816111UL, 81470997UL, 1943803523UL, 3814918930UL, 2489596804UL, 225274430UL, 2053790376UL, 3826175755UL,
    2466906013UL, 167816743UL, 2097651377UL, 4027552580UL, 2265490386UL, 503444072UL, 1762050814UL, 4150417245UL, 2154129355UL, 426522225UL,
    1852507879UL, 4275313526UL, 2312317920UL, 282753626UL, 1742555852UL, 4189708143UL, 2394877945UL, 397917763UL, 1622183637UL, 3604390888UL,
    2714866558UL, 953729732UL, 1340076626UL, 3518719985UL, 2797360999UL, 1068828381UL, 1219638859UL, 3624741850UL, 2936675148UL, 906185462UL,
    1090812512UL, 3747672003UL, 2825379669UL, 829329135UL, 1181335161UL, 3412177804UL, 3160834842UL, 628085408UL, 1382605366UL, 3423369109UL,
    3138078467UL, 570562233UL, 1426400815UL, 3317316542UL, 2998733608UL, 733239954UL, 1555261956UL, 3268935591UL, 3050360625UL, 752459403UL,
    1541320221UL, 2607071920UL, 3965973030UL, 1969922972UL, 40735498UL, 2617837225UL, 3943577151UL, 1913087877UL, 83908371UL, 2512341634UL,
    3803740692UL, 2075208622UL, 213261112UL, 2463272603UL, 3855990285UL, 2094854071UL, 198958881UL, 2262029012UL, 4057260610UL, 1759359992UL,
    534414190UL, 2176718541UL, 4139329115UL, 1873836001UL, 414664567UL, 2282248934UL, 4279200368UL, 1711684554UL, 285281116UL, 2405801727UL,
    4167216745UL, 1634467795UL, 376229701UL, 2685067896UL, 3608007406UL, 1308918612UL, 956543938UL, 2808555105UL, 3495958263UL, 1231636301UL,
    1047427035UL, 2932959818UL, 3654703836UL, 1088359270UL, 936918000UL, 2847714899UL, 3736837829UL, 1202900863UL, 817233897UL, 3183342108UL,
    3401237130UL, 1404277552UL, 615818150UL, 3134207493UL, 3453421203UL, 1423857449UL, 601450431UL, 3009837614UL, 3294710456UL, 1567103746UL,
    711928724UL, 3020668471UL, 3272380065UL, 1510334235UL, 755167117UL
};

static unsigned long hash_string_(string_s *src)
{
    char *s = src->str;
    unsigned long n, sz = (unsigned long)src->sz;
    unsigned long __attribute__ ((aligned(4))) c = 0UL;
    __builtin_prefetch(s + sz, 0,1,1);

    for (n = 0; n < sz; n++) {
        __builtin_prefetch(&s[(n+1)]);
        c = fast_crc[(c ^ s[n]) & 0xff] ^ (c >> 8);
    }
    return c ^ 0xffffffffL;
}
static int hash_char_count(char *s, char c)
{
    return ((*s == '\0') ? 0 : hash_char_count(s + 1, c) + (*s == c));
}

FUNEXPORT unsigned long hash_string(string_s *src)
{
    return hash_string_(src);
}

/* Hash Table static */

#if (defined(HAVE_BUILD_USE_QSORT) && (HAVE_BUILD_USE_QSORT == 1))
#   include "st-revizor-hash-dns2.c"
#else
#   include "st-revizor-hash-dns1.c"
#endif

/* Hash Table public */

FUNEXPORT int hash_table_check(hash_table_t *ht)
{
    return (((ht == NULL) || ((ht->arr == NULL) && (ht->ele == NULL)) || ((ht->arridx == 0) && (ht->eleidx == 0))) ? 0 : 1);
}
FUNEXPORT void hash_table_free(hash_table_t *ht)
{
    if (ht != NULL)
    {
        if (ht->arr != NULL) { free(ht->arr); }
        if (ht->ele != NULL)
        {
            for (unsigned long i = 0; i < ht->eleidx; i++) { free(ht->ele[i]); }
            free(ht->ele);
        }
        free(ht);
    }
}
FUNEXPORT hash_table_t * hash_table_dns_read(string_s *fpath)
{
    __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: begin create table from file -> %s", fpath->str);
    __cleanup_new_fclose(FILE) fp;

    if ((fp = fopen(fpath->str, "r")) == NULL)
    {
        __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: error open file -> %s (%s)", fpath->str, strerror(errno));
        __MACRO_CLEAR_HASHTABLE__(fp, NULL);
        return NULL;
    }

    hash_table_t *ht;
    if ((ht = calloc(1, sizeof(hash_table_t))) == NULL)
    {
        __MACRO_CLEAR_HASHTABLE__(fp, NULL);
        return NULL;
    }
    __cleanup_set_free(hash_table_t, htbfree, ht);

    if ((ht->arr = calloc(1001, sizeof(unsigned long))) == NULL)
    {
        __MACRO_CLEAR_HASHTABLE__(fp, ht);
        return NULL;
    }

    string_s sline = {0}, data = {0};
    char *param[3] = { NULL };
    unsigned long hash;

    ht->arridx = ht->eleidx = 0;
    ht->elemin = ht->elemax = 0;
    errno = 0;

    while (getline(&sline.str, ((size_t*)&sline.sz), fp) != -1)
    {
        if ((sline.sz < 2) || (sline.str == NULL))
        {
            continue;
        }

        string_trim(&sline);
        param[0] = param[1] = param[2] = NULL;

        if (
            (string_split((char * const)sline.str, (char**)param, __NELE(param)) < 2) ||
            (param[0] == NULL) || (param[0][0] != 'D') || (param[1] == NULL)
           )
        {
            continue;
        }

        data.str = ((param[1][0] == '.') ? (param[1] + 1) : param[1]);
        string_trim(&data);
        string_case(&data,0);
        hash = hash_string_(&data);

        if (param[1][0] == '.')
        {
            int ret = 0, cnt = hash_char_count(data.str, '.');
#           if 1
            if (cnt == 0)
            {
                __MACRO_TOSYSLOG__(LOG_ERR, STR_HASH_TBL_DEBUG "DNS: host format error %s/%d", param[1], cnt);
                continue;
            }
#           endif
            ++cnt;

            do {
                if (ht->ele == NULL)
                {
                    if ((ht->ele = calloc((ht->eleidx + 1), sizeof(hash_ele_t*))) == NULL) { ret = -1; break; }
                }
                else
                {
                    if ((ht->ele = realloc(ht->ele, ((ht->eleidx + 1) * sizeof(hash_ele_t*)))) == NULL) { ret = -1; break; }
                    if ((ht->eleidx > 0UL) && (hash_table_search_wildcard_(ht, hash, data.sz, cnt, 1) != 0UL)) { ret = -2; break; }
                }
                if ((ht->ele[ht->eleidx] = calloc(1, sizeof(hash_ele_t))) == NULL) { ret = -1; break; }

            } while(0);

            switch(ret)
            {
                case -1: { __MACRO_CLEAR_HASHTABLE__(fp, ht); return NULL; }
                case -2: { continue; }
                default: { break; }
            }

            ht->ele[ht->eleidx]->len  = data.sz;
            ht->ele[ht->eleidx]->hash = hash;
            ht->ele[ht->eleidx]->cnt  = cnt;
            ht->elemin = (((ht->ele[ht->eleidx]->cnt > ht->elemin) && (ht->elemin != 0)) ? ht->elemin : ht->ele[ht->eleidx]->cnt);
            ht->elemax = ((ht->ele[ht->eleidx]->cnt > ht->elemax) ? ht->ele[ht->eleidx]->cnt : ht->elemax);
            ht->eleidx++;
            hash_table_sort_wildcard_(ht);
            if ((ht->eleidx % 100) == 0)
            {
                __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS complette wildcard parse -> %lu hosts", ht->arridx);
            }
        }
        else
        {
            if ((ht->arr != NULL) && (ht->arridx > 0UL))
            {
                if (hash_table_search_(ht, hash) != 0UL)
                {
                    continue;
                }
                if ((ht->arridx % 1000) == 0)
                {
                    if ((ht->arr = realloc(ht->arr, ((ht->arridx + 1001) * sizeof(unsigned long)))) == NULL)
                    {
                        __MACRO_CLEAR_HASHTABLE__(fp, ht);
                        return NULL;
                    }
                    __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS complette parse -> %lu hosts", ht->arridx);
                }
            }
            memcpy((void*)&ht->arr[ht->arridx], (void*)&hash, sizeof(unsigned long));
            ht->arridx++;
            hash_table_sort_(ht);
        }
    }
    if (errno != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_HASH_TBL_DEBUG "DNS: read %s return: %s", fpath->str, strerror(errno));
    }

    if (sline.str != NULL)
    {
        free(sline.str);
    }
    __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: read from %s -> %lu hosts, %lu wilcards hosts.",
        fpath->str,
        ht->arridx,
        ht->eleidx
    );
    __MACRO_CLEAR_HASHTABLE__(fp, NULL);

    if (!hash_table_check(ht))
    {
        __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: table index empty.. %s", "Abort..");
        __MACRO_CLEAR_HASHTABLE__(-1, ht);
        return NULL;
    }
    if ((ht->arr != NULL) && (ht->arridx > 0))
    {
        if ((ht->arr = realloc(ht->arr, (ht->arridx * sizeof(unsigned long)))) == NULL)
        {
            __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: base table reallocation memory error.. %s", "Abort..");
            __MACRO_CLEAR_HASHTABLE__(-1, ht);
            return NULL;
        }
    }
    if ((ht->arridx == 0UL) && (ht->eleidx == 0UL))
    {
        __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "DNS: table data empty.. %s", "Clear..");
        __MACRO_CLEAR_HASHTABLE__(-1, ht);
    }
    htbfree = NULL;
    return ht;
}
FUNEXPORT hash_tree_response_e hash_table_dns_search(hash_table_t *ht, char *search)
{
    if ((ht == NULL) || (ht->arr == NULL) || (search == NULL))
    {
        return HASH_TREE_ERRQUERY;
    }

    unsigned long uldata, ret = 0UL;
    __BUFFER_CREATE__(data);
    data.sz = strlen(search);

    /* DNS '.' end dot remove */
    if (search[(data.sz - 1)] == '.')
    {
        --data.sz;
    }
    if (data.sz <= 0)
    {
        return HASH_TREE_ERRQUERY;
    }
    memcpy(data.str, search, data.sz);
    data.str[data.sz] = '\0';

    string_trim(&data);
    string_case(&data,0);
    uldata = hash_string_(&data);

    do {
        /* Root full name host domain check */
        if ((ht->arr == NULL) || (ht->arridx == 0)) { break; }
        ret = hash_table_search_(ht, uldata);

    } while(0);

    do {
        if ((ret) || (ht->ele == NULL) || (ht->eleidx == 0)) { break; }
        /* Wildcard any domain before root host check */
        string_s dst = {0};
        for (int i = hash_char_count(data.str, '.'); i >= ht->elemin; i--)
        {
            if (string_char_substring(&data, &dst, '.', i) != NULL)
            {
                if ((ret = hash_table_search_wildcard_(ht, hash_string_(&dst), dst.sz, i, 1)) != 0UL) { break; }
            }
        }
        if (ret) { break; }
        /* Wildcard root full name host check */
        ret = hash_table_search_wildcard_(ht, uldata, data.sz, 0, 0);

    } while(0);

    return ((!ret) ? HASH_TREE_NOTFOUND : HASH_TREE_FOUND);
}

/* Hash Tree */

#if 1

/* Hash Tree debug print */

static void hash_print_tab_(int cnt)
{
    for (int n = 0; n < cnt; n++) { putchar('\t'); }
}
FUNEXPORT void hash_tree_print(hash_tree_ele_t **htr, unsigned long idx, int cnt)
{
    if (htr == NULL)
    {
        hash_print_tab_(cnt);
        printf(" ! list: array is NULL, index: %lu\n", idx); return;
    }
    if (idx == 0)
    {
        hash_print_tab_(cnt);
        printf(" ! list: array empty, index: 0\n"); return;
    }
    for (int i = 0; i < idx; i++)
    {
        hash_print_tab_(cnt);
        printf(" %c list:%d/%lu hash:%lu\n", ((cnt == 0) ? '*' : '+'), i, idx, htr[i]->hash);
        hash_tree_print(htr[i]->tree, htr[i]->treeidx, (cnt + 1));
    }
}
#else
#   define hash_tree_print(...)
#endif

/* Hash Tree static */

static void hash_tree_ele_free_(hash_tree_ele_t **hte, int idx)
{
    if (hte == NULL)
    {
        return;
    }
    if (idx > 0)
    {
        for (int i = 0; i < idx; i++)
        {
            hash_tree_ele_free_(hte[i]->tree, hte[i]->treeidx);
            free(hte[i]);
        }
    }
    free(hte);
}

/* Hash Tree platform spcefied */

#if (defined(HAVE_BUILD_USE_QSORT) && (HAVE_BUILD_USE_QSORT == 1))
#   include "st-revizor-hash-url2.c"
#else
#   include "st-revizor-hash-url1.c"
#endif

