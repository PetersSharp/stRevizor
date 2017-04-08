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

#ifndef REVIZOR_LOCK_H
#define REVIZOR_LOCK_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../config.h"
#endif

#define __MACRO_WAITLOCK(A) \
    { int ret; \
    do { \
        __SYNC_MEM(ret = (A)); \
    } while(ret != 0); }

#define __HASH_RLOCK(A,B) \
    st_read_lock(&A); B; st_read_unlock(&A)

#define __HASH_WLOCK(A,B) \
    st_write_lock(&A); B; st_write_unlock(&A)

#define __TABLE_RLOCK(A,B) \
    st_read_lock(A); B; st_read_unlock(A)

#define __TABLE_WLOCK(A,B) \
    st_write_lock(A); B; st_write_unlock(A)

#if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
#   define __SYNC_MEM(A) __sync_synchronize(); A
#else
#   define __SYNC_MEM(A) __asm__ __volatile__ ("nop" ::: "memory"); A
#endif

#if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
#   define st_get_counter(A,B) (B) = __sync_fetch_and_add(&A, 0)
#else
#   define st_get_counter(A,B) __SYNC_MEM((B) = (A))
#endif

#if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
#   define st_add_counter(A,B) (B) = __sync_fetch_and_add(&A, 0); __sync_add_and_fetch(&A, 1)
#else
#   define st_add_counter(A,B) __SYNC_MEM((B) = (A)++)
#endif

#if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
#   define st_sub_counter(A,B) (B) = __sync_sub_and_fetch(&A, 1)
#else
#   define st_sub_counter(A,B) __SYNC_MEM((B) = --(A))
#endif

typedef struct __hash_table_lock__
{
    int r, w;
} st_table_lock_t;

void st_read_lock(st_table_lock_t*);
void st_read_unlock(st_table_lock_t*);
void st_write_lock(st_table_lock_t*);
void st_write_unlock(st_table_lock_t*);

#endif
