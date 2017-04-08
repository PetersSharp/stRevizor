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
#include "st-revizor-lock.h"

FUNEXPORT void st_read_lock(st_table_lock_t *htl)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    while(__sync_fetch_and_add(&htl->w, 0) != 0) { };
    __sync_add_and_fetch(&htl->r, 1);
#   else
    __MACRO_WAITLOCK(htl->w);
    __SYNC_MEM(htl->r++);
#   endif
}
FUNEXPORT void st_read_unlock(st_table_lock_t *htl)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    if (__sync_fetch_and_add(&htl->r, 0) > 0)
    {
        __sync_sub_and_fetch(&htl->r, 1);
    }
#   else
    __SYNC_MEM(
        if (htl->r > 0)
        {
            htl->r--;
        }
    );
#   endif
}
FUNEXPORT void st_write_lock(st_table_lock_t *htl)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    while(__sync_fetch_and_add(&htl->w, 0) != 0) { };
    __sync_add_and_fetch(&htl->w, 1);
    while(__sync_fetch_and_add(&htl->r, 0) != 0) { };
#   else
    __MACRO_WAITLOCK(htl->w);
    __SYNC_MEM(htl->w++);
    __MACRO_WAITLOCK(htl->r);
#   endif
}
FUNEXPORT void st_write_unlock(st_table_lock_t *htl)
{
#   if defined(__X_BUILD_GCC_VERSION) && (__X_BUILD_GCC_VERSION >= 40100)
    if (__sync_fetch_and_add(&htl->w, 0) > 0)
    {
        __sync_sub_and_fetch(&htl->w, 1);
    }
#   else
    __SYNC_MEM(
        if (htl->w > 0)
        {
            htl->w--;
        }
    );
#   endif
}
