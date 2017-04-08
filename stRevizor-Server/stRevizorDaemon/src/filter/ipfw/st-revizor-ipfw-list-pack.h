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

#ifndef REVIZOR_IPFW_LIST_PACK_H
#define REVIZOR_IPFW_LIST_PACK_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../../config.h"
#endif

#define __IPFW_SCR_CASEPATH \
    "\t%s)\n\t\tsource %s\n\t;;\n"

#define __IPFW_SCRSUB_TEMPLATE \
    "#!/bin/bash\n\n" \
    "# this empy template!\n" \
    "# please, insert this fwall code!\n\n" \
    "logger -t \"stRevizor-wrapper\" \"ipfw template '" HAVE_WATCH_DRIVER "/exec/*/${1}.inc' is empty! Edit script!\"\n"

#define __IPFW_LABEL_CASE(A,B) \
    case __EVALUTE_FUNC_CONCAT(IPFW_,A): { \
        context->str = CP \
            __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_, \
                __EVALUTE_FUNC_CONCAT(B, \
                    __EVALUTE_FUNC_CONCAT(_ipfw_, \
                        __EVALUTE_FUNC_CONCAT(A,_tobin_start))) \
        ); \
        context->sz  = (int)( \
            CP __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_, \
                __EVALUTE_FUNC_CONCAT(B, \
                    __EVALUTE_FUNC_CONCAT(_ipfw_, \
                        __EVALUTE_FUNC_CONCAT(A,_tobin_end)))) - \
            CP __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_, \
                __EVALUTE_FUNC_CONCAT(B, \
                    __EVALUTE_FUNC_CONCAT(_ipfw_, \
                        __EVALUTE_FUNC_CONCAT(A,_tobin_start)))) \
        ); \
        break; \
    }

#define __IPFW_LABEL_TEMPLATE(A) \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_none_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_start)); \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_none_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_end)); \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_ipt_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_start)); \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_ipt_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_end)); \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_ips_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_start)); \
    extern const char \
        __EVALUTE_FUNC_CONCAT(_binary_______src_filter_ipfw_script_ipfw_ips_ipfw_, \
            __EVALUTE_FUNC_CONCAT(A,_tobin_end));

extern const char _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_s_tobin_start;
extern const char _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_s_tobin_end;
extern const char _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_e_tobin_start;
extern const char _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_e_tobin_end;

#define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_TEMPLATE(B)
#include "st-revizor-ipfw-list-define.h"
#undef __IPFW_LABEL

#endif
