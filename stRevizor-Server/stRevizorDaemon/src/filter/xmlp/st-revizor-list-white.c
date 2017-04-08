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

#include "../../st-revizor-watch.h"
#include "../../st-revizor-inline.h"
#include "st-revizor-xml-parser.h"

static string_s whitedns[] = {
#   define __WLST_URL(A) { __ADD_string_s(A) },
#   define __WLST_IP(A)
#   include "st-revizor-list-white.h"
#   undef __WLST_URL
#   undef __WLST_IP
};

static string_s whiteip[] = {
#   define __WLST_URL(A)
#   define __WLST_IP(A) { __ADD_string_s(A) },
#   include "st-revizor-list-white.h"
#   undef __WLST_URL
#   undef __WLST_IP
};

void plist_white_list(xml_ele_t *ele)
{
    int i, n;
    INIT_NTOP();
    __ARR_TMP_INIT(wl);
    __ARR_TMP_INIT(dw);
    __ARR_TMP_INIT(tmp);

    for (i = 0; i < __NELE(whitedns); i++)
    {
        if (arr_dns_add(pwl, &whitedns[i], ptmp, CP "IP from DNS WhiteList (Part I)") == 0)
        {
            for (n = 0; n < ptmp->idx; n++)
            {
                if (arr_num_add(pdw, ptmp->arr[n], CP "IP from DNS WhiteList (Part II)") > 0UL)
                {
                    continue;
                }
                SET_NTOP(
                    ptmp->arr[n],
                    __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I, "W", ntop_ips)
                );
            }
            __cleanup_notsupport_p(auto_freeip_to_arr_t, ptmp);
        }
    }
    __cleanup_notsupport_p(auto_freeip_to_arr_t, pwl);

    for (i = 0; i < __NELE(whiteip); i++)
    {
        if (arr_ip_add(pdw, &whiteip[i], CP "IP to WhiteList") > 0UL)
        {
            __LOOP_WDATA(ele->fp, __LOOP_FORMAT_I, "W", whiteip[i].str);
        }
    }
    __cleanup_notsupport_p(auto_freeip_to_arr_t, pdw);
}
