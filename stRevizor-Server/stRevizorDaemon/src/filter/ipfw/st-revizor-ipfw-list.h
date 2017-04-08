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

#ifndef REVIZOR_IPFW_LIST_H
#define REVIZOR_IPFW_LIST_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../../config.h"
#endif

#define __IPFW_LABEL_STRUCT(A,B,C,D) \
    { .enable = (D), \
      .nele   = (C), \
      .count  = 0UL, \
      .label  = (ipfw_label_e)(__IPFW_LABEL_ENUM(B)), \
      .mark   = __IPFW_LABEL_ID(A), \
      .name   = { \
            __ADD_string_s( \
                __EVALUTE_TO_STR(B) \
            )}, \
      .nid  = {{ \
            __ADD_string_s( \
                __EVALUTE_TO_STR( \
                    __EVALUTE_FUNC_CONCAT(IPFW_, \
                        __EVALUTE_FUNC_CONCAT(NONE, \
                            __EVALUTE_FUNC_CONCAT(_,B) \
            ))))},{ \
            __ADD_string_s( \
                __EVALUTE_TO_STR( \
                    __EVALUTE_FUNC_CONCAT(IPFW_, \
                        __EVALUTE_FUNC_CONCAT(IPT, \
                            __EVALUTE_FUNC_CONCAT(_,B) \
            ))))},{ \
            __ADD_string_s( \
                __EVALUTE_TO_STR( \
                    __EVALUTE_FUNC_CONCAT(IPFW_, \
                        __EVALUTE_FUNC_CONCAT(IPS, \
                            __EVALUTE_FUNC_CONCAT(_,B) \
            ))))} \
        } \
    }

#define __IPFW_LABEL_ENUM(A) \
    __EVALUTE_FUNC_CONCAT(IPFW_,A)

#define __IPFW_LABEL_ID(A) \
    (char)(A)

#define __IPFW_LABEL_HELPER(A) \
    __EVALUTE_FUNC_CONCAT(IPFW_, \
        __EVALUTE_FUNC_CONCAT(IPS, \
            __EVALUTE_FUNC_CONCAT(_,A))), \
    __EVALUTE_FUNC_CONCAT(IPFW_, \
        __EVALUTE_FUNC_CONCAT(IPT, \
            __EVALUTE_FUNC_CONCAT(_,A))), \
    __EVALUTE_FUNC_CONCAT(IPFW_, \
        __EVALUTE_FUNC_CONCAT(NONE, \
            __EVALUTE_FUNC_CONCAT(_,A))) \

#define __CHECK_ipfw_label_t(A,B,C) \
    (( \
        ((B)[0] == (A).mark) && \
        ((B)[1] == '\0') \
     ) ? ( \
            ((A).enable) ? \
                (((C) == 0) ? 0 : 1) : (((C) == 0) ? 1 : 0) \
         ) : 0)

#define __GETTIME_EXEC(A) \
    { \
        time_t lt = time(NULL); \
        string_s tms = { ctime(&lt), 0 }; \
        string_trim(&tms); \
        A; \
    }

typedef enum __core_ipfw_label_e_ {
#   define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_ENUM(B),
#   include "st-revizor-ipfw-list-define.h"
#   undef __IPFW_LABEL
    IPFW_st_revizor_ipfw_script_s,
    IPFW_st_revizor_ipfw_script_e
} ipfw_label_e;

/*
    // Template sruct in .c file

ipfw_label_t ipfwl[] = {
#    define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_STRUCT(A,B,C,D)
#    include "st-revizor-ipfw-list-define.h"
#    undef __IPFW_LABEL
};

    // Template Helper to .sh label

char *helper = "\""
#    define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_HELPER(B)
#    include "st-revizor-ipfw-list-define.h"
#    undef __IPFW_LABEL
"\""
;

*/

typedef struct __core_ipfw_label_t_ {
    int           enable, nele;
    unsigned long count;
    char          mark;
    ipfw_label_e  label;
    string_s      name;
    string_s      nid[3];
} ipfw_label_t;

void   ipfw_exec(ipfw_label_e, char**);
void   ipfw_parse(string_s);
void * ipfw_parse_line(string_s*);
void   ipfw_stat_prn(void);
void   ipfw_env_prn(void);
void   ipfw_pack_check(void);
ipfw_label_t * ipfw_stat_get(ipfw_label_e);

#endif
