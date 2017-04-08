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

#include "st-revizor-watch.h"
#include "st-revizor-inline.h"
#include "st-revizor-iptables.h"

#define __IPT iptables_
#define __IPS ipset_

extern string_s insource[];
extern int inprocess[];
extern nfbin_t binpath;
extern watcher_t wrun;

const char *ipt_chain_flags2[] = { "F","X" };
const char *ipt_chain_flags3[] = { "F","X","N" };

    __MACRO_INITWRAP__();

#if defined(HAVE_IPS_ENABLE) && (HAVE_IPS_ENABLE == 1)
    const char *ipt_chain_name[]  = { IPT_CHAINMAIN, IPT_CHAINURL };
    const char *ips_table_name[]  = { IPT_CHAINWHT, IPT_CHAINIP };
#   define __UNIQ(A) (__EV(__IPS,A))
#   include "include/st-revizor-ips.c"
#else
    const char *ipt_chain_name[]  = { IPT_CHAINMAIN, IPT_CHAINIP, IPT_CHAINURL };
#   define __UNIQ(A) (__EV(__IPT,A))
#   include "include/st-revizor-ipt.c"
#endif

FUNEXPORT void main_module_parse(string_s fpath, int type)
{
    __MACRO_CREATEBIN__();
    switch(type)
    {
        case 2: {
            file_read(fpath, bpath, __UNIQ(biplist));
            break;
        }
        case 3: {
            file_read(fpath, bpath, __UNIQ(urllist));
            break;
        }
        case 4: {
            file_read(fpath, bpath, __UNIQ(wiplist));
            break;
        }
        case 5: {
            __UNIQ(old_portlist_clear)(bpath, 5);
            file_read(fpath, bpath, __UNIQ(portlist));
            break;
        }
        case 6: {
            file_read(fpath, bpath, __UNIQ(piplist));
            break;
        }
        case 7: {
            file_backup_fp(fpath, 7);
            kill(wrun.pid, SIGUSR2);
            break;
        }
        default: {
            return;
        }
    }
}
FUNEXPORT void main_module_start(void)
{
    if (access(IPT_WRAPPER, F_OK) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: (Netfilter) wrapper %s, %s",
            IPT_WRAPPER,
            strerror(errno)
        );
        __MACRO_SETWRAP__(0);
    }
    __MACRO_SETWRAP__(1);
    __UNIQ(start)();
}
FUNEXPORT void main_module_end(void)
{
    __MACRO_CHECKWRAP__();
    __UNIQ(end)();
}
FUNEXPORT void main_module_clear(void)
{
    __MACRO_CHECKWRAP__();
    __UNIQ(clear)();
}
FUNEXPORT void main_module_dns(int sel)
{
    char *ipt_flags[][2] = {
        { "D", NULL },
        { "D", "I" }
    };
    nfbin_t bpath = {0};
    sel = ((sel > 0) ? 1 : 0);

    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPT);

    for (int i = 0; i < __NELE(ipt_flags[sel]); i++)
    {
        if (ipt_flags[sel][i] == NULL)
        {
            break;
        }
        __MACRO_IPTRULE__(bpath, fwall, , IPT_DNS_FILTER1, ipt_flags[sel][i], HAVE_IPT_NET, HAVE_IPT_IFACE, HAVE_DNS_CLPORT);
#       if defined(HAVE_DNS_LOCAL_RULE) && (HAVE_DNS_LOCAL_RULE == 1)
        __MACRO_IPTRULE__(bpath, fwall, , IPT_DNS_FILTER2, ipt_flags[sel][i], HAVE_DNS_CLPORT);
#       endif
    }
    __MACRO_TOSCREEN__("[Net Filter]: %s DNS rules, redirect: udp port 53 -> %d", ((sel) ? "add" : "delete"), HAVE_DNS_CLPORT);
}
