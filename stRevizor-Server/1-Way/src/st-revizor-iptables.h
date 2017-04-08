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

#ifndef REVIZOR_IPTABLES_H
#define REVIZOR_IPTABLES_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../config.h"
#endif

#define IPT_EMPTY " "

#define IPT_WRAPPER "/usr/sbin/revizor-wrapper.sh"

#define IPT_RULE_REJECT "REJECT"
#define IPT_RULE_RETURN "RETURN"
#define IPT_RULE_DROP   "DROP"

#define IPT_CHAINMAIN "REVIZOR"
#define IPT_CHAINIP   "REVIZORIP"
#define IPT_CHAINURL  "REVIZORURL"
#define IPT_CHAINEND  "REVIZOREND"
#define IPT_CHAINFWD  "FORWARD"
#define IPT_CHAINIPP  "REVIZORPORT"
#define IPT_CHAINWHT  "REVIZORWHT"

#define IPT_IPPLIST     "-t filter -%s " IPT_CHAINIPP "%s -d %s -j " IPT_CHAINEND
#define IPT_IPLIST      "-t filter -%s %sTMP -d %s -j %s"
#define IPT_URLLIST     "-t filter -%s %sTMP -m string --string \"%s\" --algo kmp --icase -j %s"
#define IPT_FILTERIPP   "-t filter -%s %sTMP -p tcp --dport %s -m conntrack --ctstate NEW -j " IPT_CHAINIPP "%s"

#define IPS_FILTERIPP   "-t filter -%s %sTMP -p tcp --dport %s -m conntrack --ctstate NEW -m set --match-set %s%s dst -j %s"
#define IPS_IPPLIST     "-t filter -%s %sTMP -m conntrack --ctstate NEW -m set --match-set %s dst -j %s"
#define IPS_WHTLIST     "-t filter -%s %sTMP -m set --match-set %s dst -j ACCEPT"

#define IPT_FILTERIP    "-t filter -%s %s -m conntrack --ctstate NEW -j %s"
#define IPT_FILTERURL   "-t filter -%s %s -p tcp -m conntrack --ctstate NEW -j %s"
#define IPT_TOFORWARD   "-t filter -%s %s -i %s -s %s -j %s"


#define IPT_DNS_FILTER1 "-t nat -%s PREROUTING -s %s -i %s -p udp --dport 53 -j REDIRECT --to-port %d"
#define IPT_DNS_FILTER2 "-t nat -%s OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port %d"

#define IPS_CREATE     "create %s%s hash:ip family inet hashsize 1024 maxelem 65536"
#define IPS_ADD        "add %s%s %s"
#define IPS_FLUSH      "flush %s%s"
#define IPS_DELETE     "-X %s%s"

#define IPT_CHAIN       "-t filter -%s %s"
#define IPT_CHAINPIP    "-t filter -%s %s%s"
#define IPT_CHAINTMP    "-t filter -%s %sTMP"
#define IPT_CHAINRENAME "-t filter -E %sTMP %s"
#define IPT_CHAINFINAL  "-t filter -A %s -j %s"

#define __MACRO_INITWRAP__() \
    static volatile int chkwrep = 0

#define __MACRO_CHECKWRAP__() \
    if (!chkwrep) { return; }

#define __MACRO_SETWRAP__(A) \
    { chkwrep = A; }

#define __MACRO_CREATEBIN__() \
    nfbin_t bpath = {0}

#define __MACRO_SETBIN__(A) \
    bpath.bin = CP IPT_WRAPPER; \
    if (A == ENUMBIN_IPT) { \
        memcpy(&bpath.ipt, &binpath.ipt, sizeof(string_s)); \
        bpath.ips.str = CP IPT_EMPTY; \
    } else if (A == ENUMBIN_IPS) { \
        memcpy(&bpath.ips, &binpath.ips, sizeof(string_s)); \
        bpath.ipt.str = CP IPT_EMPTY; \
    } else { \
        bpath.ips.str = CP IPT_EMPTY; \
        bpath.ipt.str = CP IPT_EMPTY; \
    }


#endif
