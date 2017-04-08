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
    1996959894, 3993919788, 2567524794, 124634137, 1886057615, 3915621685, 2657392035, 249268274, 2044508324, 3772115230,
    2547177864, 162941995, 2125561021, 3887607047, 2428444049, 498536548, 1789927666, 4089016648, 2227061214, 450548861,
    1843258603, 4107580753, 2211677639, 325883990, 1684777152, 4251122042, 2321926636, 335633487, 1661365465, 4195302755,
    2366115317, 997073096, 1281953886, 3579855332, 2724688242, 1006888145, 1258607687, 3524101629, 2768942443, 901097722,
    1119000684, 3686517206, 2898065728, 853044451, 1172266101, 3705015759, 2882616665, 651767980, 1373503546, 3369554304,
    3218104598, 565507253, 1454621731, 3485111705, 3099436303, 671266974, 1594198024, 3322730930, 2970347812, 795835527,
    1483230225, 3244367275, 3060149565, 1994146192, 31158534, 2563907772, 4023717930, 1907459465, 112637215, 2680153253,
    3904427059, 2013776290, 251722036, 2517215374, 3775830040, 2137656763, 141376813, 2439277719, 3865271297, 1802195444,
    476864866, 2238001368, 4066508878, 1812370925, 453092731, 2181625025, 4111451223, 1706088902, 314042704, 2344532202,
    4240017532, 1658658271, 366619977, 2362670323, 4224994405, 1303535960, 984961486, 2747007092, 3569037538, 1256170817,
    1037604311, 2765210733, 3554079995, 1131014506, 879679996, 2909243462, 3663771856, 1141124467, 855842277, 2852801631,
    3708648649, 1342533948, 654459306, 3188396048, 3373015174, 1466479909, 544179635, 3110523913, 3462522015, 1591671054,
    702138776, 2966460450, 3352799412, 1504918807, 783551873, 3082640443, 3233442989, 3988292384, 2596254646, 62317068,
    1957810842, 3939845945, 2647816111, 81470997, 1943803523, 3814918930, 2489596804, 225274430, 2053790376, 3826175755,
    2466906013, 167816743, 2097651377, 4027552580, 2265490386, 503444072, 1762050814, 4150417245, 2154129355, 426522225,
    1852507879, 4275313526, 2312317920, 282753626, 1742555852, 4189708143, 2394877945, 397917763, 1622183637, 3604390888,
    2714866558, 953729732, 1340076626, 3518719985, 2797360999, 1068828381, 1219638859, 3624741850, 2936675148, 906185462,
    1090812512, 3747672003, 2825379669, 829329135, 1181335161, 3412177804, 3160834842, 628085408, 1382605366, 3423369109,
    3138078467, 570562233, 1426400815, 3317316542, 2998733608, 733239954, 1555261956, 3268935591, 3050360625, 752459403,
    1541320221, 2607071920, 3965973030, 1969922972, 40735498, 2617837225, 3943577151, 1913087877, 83908371, 2512341634,
    3803740692, 2075208622, 213261112, 2463272603, 3855990285, 2094854071, 198958881, 2262029012, 4057260610, 1759359992,
    534414190, 2176718541, 4139329115, 1873836001, 414664567, 2282248934, 4279200368, 1711684554, 285281116, 2405801727,
    4167216745, 1634467795, 376229701, 2685067896, 3608007406, 1308918612, 956543938, 2808555105, 3495958263, 1231636301,
    1047427035, 2932959818, 3654703836, 1088359270, 936918000, 2847714899, 3736837829, 1202900863, 817233897, 3183342108,
    3401237130, 1404277552, 615818150, 3134207493, 3453421203, 1423857449, 601450431, 3009837614, 3294710456, 1567103746,
    711928724, 3020668471, 3272380065, 1510334235, 755167117
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
static void hash_sort_(hash_table_t *ht)
{
    unsigned long i, low, val1, val2, *arr0 = ht->arr;
    for (low = 0; low < (ht->idx - 1); low++)
    {
        for (i = 0; i < (ht->idx - low - 1); i++)
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
static unsigned long hash_search_(hash_table_t *ht, unsigned long hash)
{
    unsigned long hi = ht->idx, low = 0UL;
    //__builtin_prefetch(ht->arr + (ht->idx * sizeof(unsigned long)), 0,1,1);
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

int  hash_check(hash_table_t *ht)
{
    return (((ht == NULL) || (ht->arr == NULL) || (ht->idx == 0)) ? 0 : 1);
}
void hash_free(hash_table_t *ht)
{
    if (ht != NULL)
    {
        if (ht->arr != NULL) { free(ht->arr); }
        free(ht);
    }
}
hash_table_t * hash_read(string_s *fpath)
{
    hash_table_t *ht;
    if ((ht = calloc(1, sizeof(hash_table_t))) == NULL)
    {
        return NULL;
    }
    if ((ht->arr = calloc(100, sizeof(unsigned long))) == NULL)
    {
        hash_free(ht); return NULL;
    }

    FILE *fp;
    string_s data = {0};
    ht->idx = 0;

    __MACRO_TOSCREEN__(STR_HASH_DEBUG "Begin create table from file -> %s", fpath->str);

    if ((fp = fopen(fpath->str, "r")) != NULL)
    {
        while (getline(&data.str, ((size_t*)&data.sz), fp) != -1)
        {
            if ((ht->idx % 100) == 0)
            {
                if ((ht->arr = realloc(ht->arr, ((ht->idx + 100) * sizeof(unsigned long)))) == NULL)
                {
                    fclose(fp); hash_free(ht); return NULL;
                }
            }
            string_trim(&data);
            string_case(&data,0);
            {
                ht->arr[ht->idx] = hash_string_(&data);
                ht->idx++;
            };
        }
        if (data.str != NULL)
        {
            free(data.str);
        }
        __MACRO_TOSCREEN__(STR_HASH_DEBUG "Read from %s -> %lu hosts..", fpath->str, ht->idx);
        fclose(fp);
    }
    else
    {
        __MACRO_TOSCREEN__(STR_HASH_DEBUG "Error open file -> %s (%s)", fpath->str, strerror(errno));
        hash_free(ht); return NULL;
    }
    if (ht->idx == 0)
    {
        __MACRO_TOSCREEN__(STR_HASH_DEBUG "Table index empty.. %s", "Abort..");
        hash_free(ht); return NULL;
    }
    if ((ht->arr = realloc(ht->arr, (ht->idx * sizeof(unsigned long)))) == NULL)
    {
        __MACRO_TOSCREEN__(STR_HASH_DEBUG "Table realloc memory error.. %s", "Abort..");
        hash_free(ht); return NULL;
    }

    __MACRO_TOSCREEN__(STR_HASH_DEBUG "Sort table.. %s", "please wait..");

    hash_sort_(ht);
    return ht;
}
unsigned long hash_search(hash_table_t *ht, string_s *search)
{
    if ((ht == NULL) || (ht->arr == NULL) || (search == NULL))
    {
        return 0UL;
    }
    string_trim(search);
    string_case(search,0);
    return hash_search_(ht, hash_string_(search));
}
unsigned long hash_search_uniq(hash_table_t *ht, char *search)
{
    if ((ht == NULL) || (ht->arr == NULL) || (search == NULL))
    {
        return 0UL;
    }
    __BUFFER_CREATE__(data);
    data.sz = strlen(search);
    memcpy(data.str, search, data.sz);

    string_trim(&data);
    string_case(&data,0);
    return hash_search_(ht, hash_string_(&data));
}
void hash_read_lock(hash_table_lock_t *htl)
{
    while(__sync_fetch_and_add(&htl->w, 0) != 0) { };
    __sync_add_and_fetch(&htl->r, 1);
}
void hash_read_unlock(hash_table_lock_t *htl)
{
    while(__sync_fetch_and_add(&htl->w, 0) != 0) { };
    __sync_sub_and_fetch(&htl->r, 1);
}
void hash_write_lock(hash_table_lock_t *htl)
{
    while(__sync_fetch_and_add(&htl->r, 0) != 0) { };
    __sync_add_and_fetch(&htl->w, 1);
}
void hash_write_unlock(hash_table_lock_t *htl)
{
    while(__sync_fetch_and_add(&htl->r, 0) != 0) { };
    __sync_sub_and_fetch(&htl->w, 1);
}
