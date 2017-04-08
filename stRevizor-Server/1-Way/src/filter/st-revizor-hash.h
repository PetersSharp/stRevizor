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

#define STR_HASH_DEBUG "[Hash Table]: ? "

typedef struct __hash_table__
{
    unsigned long  idx;
    unsigned long *arr;
} hash_table_t;

typedef struct __hash_table_lock__
{
    int r, w;
} hash_table_lock_t;

int            hash_check(hash_table_t*);
void           hash_free(hash_table_t*);
hash_table_t * hash_read(string_s*);
unsigned long  hash_search(hash_table_t*, string_s*);
unsigned long  hash_search_uniq(hash_table_t*, char*);

void hash_read_lock(hash_table_lock_t*);
void hash_read_unlock(hash_table_lock_t*);
void hash_write_lock(hash_table_lock_t*);
void hash_write_unlock(hash_table_lock_t*);

#endif
