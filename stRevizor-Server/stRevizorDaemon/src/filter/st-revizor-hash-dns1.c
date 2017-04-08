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

static void hash_table_sort_(hash_table_t *ht)
{
    unsigned long i, low, val1, val2, *arr0 = ht->arr;
    for (low = 0; low < ht->arridx; low++)
    {
        for (i = 0; i < (ht->arridx - low); i++)
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
static void hash_table_sort_wildcard_(hash_table_t *ht)
{
    unsigned long i, low;
    hash_ele_t *val1, *val2, **ele0 = ht->ele;
    for (low = 0; low < ht->eleidx; low++)
    {
        for (i = 0; i < (ht->eleidx - low); i++)
        {
            __builtin_prefetch(ele0[(i+1)]);
            if (ele0[i]->hash > ele0[(i+1)]->hash)
            {
                val1        = ele0[i];
                val2        = ele0[(i+1)];
                ele0[i]     = val2;
                ele0[(i+1)] = val1;
            }
        }
    }
}
static unsigned long hash_table_search_(hash_table_t *ht, unsigned long hash)
{
    unsigned long hi = ht->arridx, low = 0UL;
    __builtin_prefetch(ht->arr + (ht->arridx * sizeof(unsigned long)), 0,1,1);
    while (low < hi)
    {
        unsigned long mid = ((hi + low) / 2);
        if (!ht->arr[mid])             { continue; }
        else if (ht->arr[mid] == hash) { return ht->arr[mid]; }
        else if (ht->arr[mid] > hash)  { hi  = mid; }
        else                           { low = (mid + 1); }
    }
    return 0UL;
}
static unsigned long hash_table_search_wildcard_(hash_table_t *ht, unsigned long hash, int len, int cnt, int isext)
{
    unsigned long hi = ht->eleidx, low = 0UL;
    __builtin_prefetch(ht->ele + (ht->eleidx * sizeof(hash_ele_t)), 0,1,1);
    while (low < hi)
    {
        unsigned long mid = ((hi + low) / 2);
        if (!ht->ele[mid])                   { continue; }
        else if (ht->ele[mid]->hash == hash) {
            return ((isext) ?
                (((ht->ele[mid]->cnt == cnt) && (ht->ele[mid]->len == len)) ? ht->ele[mid]->hash : 0UL) :
                ht->ele[mid]->hash
            );
        }
        else if (ht->ele[mid]->hash > hash)  { hi  = mid; }
        else                                 { low = (mid + 1); }
    }
    return 0UL;
}
