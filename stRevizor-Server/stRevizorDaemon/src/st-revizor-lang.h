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

#ifndef REVIZOR_LANG_H
#define REVIZOR_LANG_H

#define STR_ACL_DEBUG       "[ACL wrapper]:\t! "
#define STR_WATCH_ERROR     "[Watch error]:\t! "
#define STR_WATCH_ARUN      "[Already running]:\t* pid: %d -> file found: %s/%s, Exit.."
#define STR_WATCH_ISRUN     "[RunTime check]:\t* not read %s -> %s"
#define STR_WATCH_START     "[Main start]:\t+ Start %s success -> pid %d"
#define STR_WATCH_STOP      "[Main stop]:\t+ Clear pid file -> %s/%s, Exit.."
#define STR_WATCH_RDIR      "[Found source]:\t+ %s"
#define STR_WATCH_CHILD     "[Child status]:\t? "
#define STR_WATCH_DEBUG     "[Watch status]:\t? "
#define STR_WATCH_BUF_ERROR "[Buffer error]:\t! "
#define STR_HASH_TBL_DEBUG  "[Hash table]:\t? "
#define STR_HASH_TREE_DEBUG "[Hash tree]:\t? "
#define STR_NET_ERROR       "[Net filter]:\t! "
#define STR_XML_ERROR       "[XML Parser]:\t! "
#define STR_XML_INFO        "[XML Parser]:\t? "
#define STR_IPT_UPDATE      "[Update fwall]:\t+ %s"
#define STR_IPT_CHECK       "[Vulnerability]: broken string: [%s]"
#define STR_IPT_EXEC        "[Exec fwall]:\t! exec '%s': %s"
#define STR_IPT_FORK        "[Exec fwall]:\t+ fork: %s"
#define STR_DNS_DEBUG       "[DNS filter]:\t? "
#define STR_URL_DEBUG       "[URL filter]:\t? "
#define STR_IPFW_INFO       "[IPFW filter]:\t+ "
#define STR_IPFW_ERROR      "[IPFW filter]:\t! "
#define STR_NETINIT_ERROR   "[Net filter]:\t! %s failed: %s"
#define STR_NET_DEBUG       "[Net filter]:\t* Debug -> %s: "
#define STR_POLL_ERROR      "[Net %s poll]:\t! Error -> %s: "
#define STR_POLL_INF_STAT   "[Net %s poll]:\t* Status -> %s: "
#define STR_POLL_INF_DEBUG  "[Net %s poll]:\t* Debug -> %s: "
#define STR_POLL_EV_DEBUG   "[Net %s poll]:\t%c Event %s -> idx:%d, for:%d, poll:%d, fd:%d = %s"
#define STR_POLL_URL_DEBUG  "[Net URL poll]:\t%c Event %s -> poll:%d, fd:%d = %s"
#define STR_POLL_EX_DEBUG   "[Net %s poll]:\t* Event pulse -> %s = queue wait: %d, total: %d"
#define STR_DNS_CHECK       "[DNS check]:\t? "

#endif

