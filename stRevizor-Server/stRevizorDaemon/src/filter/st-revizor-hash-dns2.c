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

/* Hash Table */

static int arr_cmpl(const void *a, const void *b)
{
    if (*(unsigned long*)a <  *(unsigned long*)b) { return -1; }
    if (*(unsigned long*)a == *(unsigned long*)b) { return  0; }
    if (*(unsigned long*)a >  *(unsigned long*)b) { return  1; }
    return 0;
}
static int arr_cmpw(const void *a, const void *b)
{
    if ((*(hash_ele_t**)a)->hash <  (*(hash_ele_t**)b)->hash) { return -1; }
    if ((*(hash_ele_t**)a)->hash == (*(hash_ele_t**)b)->hash) { return  0; }
    if ((*(hash_ele_t**)a)->hash >  (*(hash_ele_t**)b)->hash) { return  1; }
    return 0;
}
static int arr_cmpws(const void *a, const void *b)
{
    if (*(unsigned long*)a <  (*(hash_ele_t**)b)->hash) { return -1; }
    if (*(unsigned long*)a == (*(hash_ele_t**)b)->hash) { return  0; }
    if (*(unsigned long*)a >  (*(hash_ele_t**)b)->hash) { return  1; }
    return 0;
}
static void hash_table_sort_(hash_table_t *ht)
{
    qsort((void*)ht->arr, ht->arridx, sizeof(unsigned long), &arr_cmpl);
}
static void hash_table_sort_wildcard_(hash_table_t *ht)
{
    qsort((void*)ht->ele, ht->eleidx, sizeof(hash_ele_t*), &arr_cmpw);
}
static unsigned long hash_table_search_(hash_table_t *ht, unsigned long hash)
{
    if ((ht == NULL) || (ht->arridx == 0UL) || (hash == 0UL)) { return 0UL; }
    unsigned long lout, *data;

    if ((data = bsearch(
        (const void*)&hash,
        (const void*)ht->arr,
        ht->arridx,
        sizeof(unsigned long),
        &arr_cmpl)
    ) == NULL) { lout = 0UL; }
    else       { lout = *(data); }
    return lout;
}
static unsigned long hash_table_search_wildcard_(hash_table_t *ht, unsigned long hash, int len, int cnt, int isext)
{
    if ((ht == NULL) || (ht->eleidx == 0UL) || (hash == 0UL)) { return 0UL; }
    hash_ele_t **data;

    if ((data = bsearch(
        (const void*)&hash,
        (const void*)ht->ele,
        ht->eleidx,
        sizeof(hash_ele_t*),
        &arr_cmpws)
    ) != NULL) {
        return ((isext) ?
            ((((*data)->cnt == cnt) && ((*data)->len == len)) ? (*data)->hash : 0UL) :
            (*data)->hash
        );
    }
    return 0UL;
}
