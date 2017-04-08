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
#include "../st-revizor-lock.h"
#include "st-revizor-ipfw-list.h"

#include <sched.h>
#include <time.h>

#define __IPFW_IPT_BINSTR "REVIZOR_IPT_BIN="
#define __IPFW_IPS_BINSTR "REVIZOR_IPS_BIN="
#define __IPFW_WRAPPER "revizor-wrapper.sh"

static const char *envp[34] = {0};

string_s execwrap    = {0};
ipfw_label_t ipfwl[] = {
#    define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_STRUCT(A,B,C,D),
#    include "st-revizor-ipfw-list-define.h"
#    undef __IPFW_LABEL
};
int ipfws = __NELE(ipfwl);

static int __ipfw_checkbin(void)
{
    if (!file_exists_fp(&execwrap))
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "(Netfilter) ipfw wrapper %s, %s",
            execwrap.str,
            strerror(errno)
        );
        return 0;
    }
    return 1;
}
static int __ipfw_setbin(char *path1, char *path2)
{
    __BUFFER_CREATE__(isbin);
    __SET_string_s(isbin, path1);

    if (file_exists_fp(&isbin))
    {
        return 1;
    }
    else
    {
        __SET_string_s(isbin, path2);
        if (file_exists_fp(&isbin))
        {
            return 2;
        }
    }
    return 0;
}
static void ipfw_stat_clear_(void)
{
    for (int i = 0; i < __NELE(ipfwl); i++)
    {
        ipfwl[i].count = 0;
    }
}
static void __ipfw_ctors_init(void)
{
    int cnt = 2;

#   if (HAVE_IPFW == 0)
#   warning "bad IPFW mode, run './configure' or 'make menuconfig' again.."
    cnt = 0;
    envp[0]  = NULL;
    envp[1]  = NULL;

    // ipset selector:
#   elif ((HAVE_IPFW == 2) || (HAVE_IPFW == 4))
    switch(__ipfw_setbin(CP HAVE_IPS_BIN, CP HAVE_FIND_IPS_BIN))
    {
        case 1:
        {
            envp[1] = CP __IPFW_IPS_BINSTR HAVE_IPS_BIN;
            break;
        }
        case 2:
        {
            envp[1] = CP __IPFW_IPS_BINSTR HAVE_FIND_IPS_BIN;
            break;
        }
        default:
        {
            envp[1] = CP __IPFW_IPS_BINSTR;
            break;
        }
    }
    // include user side (HAVE_IPFW == 5)
#   else
    envp[1]  = CP __IPFW_IPS_BINSTR;
#   endif

    // iptables selector:
#   if ((HAVE_IPFW > 0) && (HAVE_IPFW < 5))
    switch(__ipfw_setbin(CP HAVE_IPT_BIN, CP HAVE_FIND_IPT_BIN))
    {
        case 1:
        {
            envp[0] = CP __IPFW_IPT_BINSTR HAVE_IPT_BIN;
            break;
        }
        case 2:
        {
            envp[0] = CP __IPFW_IPT_BINSTR HAVE_FIND_IPT_BIN;
            break;
        }
        default:
        {
            envp[0] = CP __IPFW_IPT_BINSTR;
            break;
        }
    }
    // include user side (HAVE_IPFW == 5)
#   elif (HAVE_IPFW > 0)
    envp[0]  = CP __IPFW_IPT_BINSTR;
#   endif

    envp[cnt++]  = CP "REVIZOR_DNS_CLPORT="  __EVALUTE_TO_STR(HAVE_DNS_CLPORT);
    envp[cnt++]  = CP "REVIZOR_URL_CLPORT="  __EVALUTE_TO_STR(HAVE_HTTP_CLPORT);
    envp[cnt++]  = CP "REVIZOR_DNS_FILTRED=" HAVE_DNS_FILTRED;
    envp[cnt++]  = CP "REVIZOR_URL_FILTRED=" HAVE_HTTP_FILTRED;
    envp[cnt++]  = CP "REVIZOR_IPFW_IFACE="  HAVE_IPFW_IFACE;
    envp[cnt++]  = CP "REVIZOR_IPFW_NET="    HAVE_IPFW_NET;
    envp[cnt++]  = CP "REVIZOR_IPFW_GW="     HAVE_IPFW_GW;
    envp[cnt++]  = CP "REVIZOR_IPFW_FINAL="  HAVE_IPFW_FINAL;
    envp[cnt++] = CP "REVIZOR_DNS_CLEAR="   __EVALUTE_TO_STR(HAVE_IPFW_CLEAR_RULE);
    envp[cnt++] = CP "REVIZOR_URL_CLEAR="   __EVALUTE_TO_STR(HAVE_HTTP_CLEAR_RULE);
    envp[cnt++] = CP "REVIZOR_DIR_DRIVER="  HAVE_WATCH_DRIVER;
    envp[cnt++] = CP "REVIZOR_DIR_BACKUP="  HAVE_WATCH_BACKUP;
    envp[cnt++] = CP "REVIZOR_LOG_ENABLE="  __EVALUTE_TO_STR(HAVE_WRAPPER_LOG_ENABLE);

#   if defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)
    envp[cnt++] = CP "REVIZOR_DNS_ENABLE=1";
#   else
    envp[cnt++] = CP "REVIZOR_DNS_ENABLE=0";
#   endif

#   if defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)
    envp[cnt++] = CP "REVIZOR_URL_ENABLE=1";
#   else
    envp[cnt++] = CP "REVIZOR_URL_ENABLE=0";
#   endif

#   if defined(HAVE_DNS_TCP_REJECT) && (HAVE_DNS_TCP_REJECT == 1)
    envp[cnt++] = CP "REVIZOR_DNS_TCP=1";
#   else
    envp[cnt++] = CP "REVIZOR_DNS_TCP=0";
#   endif

#   if defined(HAVE_DNS_LOCAL_RULE) && (HAVE_DNS_LOCAL_RULE == 1)
    envp[cnt++] = CP "REVIZOR_IPFW_DNS_LOCALR=1";
#   else
    envp[cnt++] = CP "REVIZOR_IPFW_DNS_LOCALR=0";
#   endif

#   if defined(HAVE_HTTP_LOCAL_RULE) && (HAVE_HTTP_LOCAL_RULE == 1)
    envp[cnt++] = CP "REVIZOR_IPFW_URL_LOCALR=1";
#   else
    envp[cnt++] = CP "REVIZOR_IPFW_URL_LOCALR=0";
#   endif

    envp[cnt++] = CP "REVIZOR_FILE_BLACKLIST=" HAVE_WATCH_BACKUP "/" TFTP_FILE_BLIST;
    envp[cnt++] = CP "REVIZOR_FILE_PORTLIST=" HAVE_WATCH_BACKUP "/" TFTP_FILE_PLIST;

#   if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
    envp[cnt++] = CP "REVIZOR_FILE_SQUID_HTTP=" HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTP;
    envp[cnt++] = CP "REVIZOR_FILE_SQUID_HTTPS=" HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTPS;
    envp[cnt++] = CP "REVIZOR_FILE_SQUID_PORT_HTTP=" HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTP;
    envp[cnt++] = CP "REVIZOR_FILE_SQUID_PORT_HTTPS=" HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTPS;

#      if defined(HAVE_HTTP_PROXY_BIN)
        envp[cnt++] = CP "REVIZOR_PROXY_BIN=" HAVE_HTTP_PROXY_BIN;
#      endif
#      if defined(HAVE_HTTP_PROXY_HOST)
        envp[cnt++] = CP "REVIZOR_PROXY_HOST=" HAVE_HTTP_PROXY_HOST;
#      endif
#      if defined(HAVE_HTTP_PROXY_PORT)
        envp[cnt++] = CP "REVIZOR_PROXY_HTTP_PORT=" __EVALUTE_TO_STR(HAVE_HTTP_PROXY_PORT);
#      endif
#      if defined(HAVE_HTTP_PROXY_TPORT)
        envp[cnt++] = CP "REVIZOR_PROXY_HTTP_TPORT=" __EVALUTE_TO_STR(HAVE_HTTP_PROXY_TPORT);
#      endif
#      if defined(HAVE_HTTPS_PROXY_TPORT)
        envp[cnt++] = CP "REVIZOR_PROXY_HTTPS_TPORT=" __EVALUTE_TO_STR(HAVE_HTTPS_PROXY_TPORT);
#      endif
#      if defined(HAVE_FIND_TFTP_BIN)
        envp[cnt++] = CP "REVIZOR_TFTP_BIN=" HAVE_FIND_TFTP_BIN;
#      endif

#   endif

    envp[cnt++] = NULL;

#   if !defined(HAVE_WATCH_DRIVER)
#     error "Revizor lib directory not defined! (variable HAVE_WATCH_DRIVER)"
#   else

    execwrap.str = CP (HAVE_WATCH_DRIVER "/exec/" __IPFW_WRAPPER);
    execwrap.sz  = sizeof(HAVE_WATCH_DRIVER "/exec/" __IPFW_WRAPPER);

#   endif
}
#if defined(__X_BUILD_ASM86) && defined(__GNUC__)
    void (*initipfwmod)(void) ELF_SECTION_ATR(".ctors") = __ipfw_ctors_init;
#endif

void ipfw_env_prn(void)
{
    for (int i = 0; i < __NELE(envp); i++)
    {
        if (envp[i] != NULL)
        {
            printf("\t\t%s\n", envp[i]);
        }
    }
}

static int ipfw_select_(ipfw_label_e label, char **param, char **data)
{
    switch(label)
    {
//#       if (defined(HAVE_BGP_FILTER_ENABLE) && (HAVE_BGP_FILTER_ENABLE == 1))
        case IPFW_BGP:
//#       endif
        case IPFW_WHITE:
        case IPFW_REJECT: {
            param[2] = ((data != NULL) ? data[1] : NULL);
            param[3] = NULL;
            break;
        }
//#       if (defined(HAVE_BGP_FILTER_ENABLE) && (HAVE_BGP_FILTER_ENABLE == 1))
        case IPFW_BGPADD:
        case IPFW_BGPDEL:
//#       endif
#       if (defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1))
        case IPFW_DNSADD:
        case IPFW_DNSDEL:
#       endif
#       if (defined(HAVE_IPFW_CLEAR_RULE) && (HAVE_IPFW_CLEAR_RULE == 1))
        case IPFW_CLEAR:
#       endif
#       if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
        case IPFW_SQUID:
#       endif
        case IPFW_LSTSTART:
        case IPFW_LSTEND: {
            param[2] = NULL;
            break;
        }
#       if (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1))
        case IPFW_URLADD:
        case IPFW_URLDEL:
#       endif
        case IPFW_CTRLSTART:
        case IPFW_CTRLEND: {
            param[2] = ((data != NULL) ? data[0] : NULL);
            param[3] = NULL;
            break;
        }
        case IPFW_DNS:
        case IPFW_URL: {
            return 0;
        }
        default: {
            return 0;
        }
    }
    return 1;
}
static void ipfw_exec_(string_s *exec, char **data)
{
    pid_t p = fork();
    if (!p)
    {
#       if (defined(HAVE_EXECVPE) && (HAVE_EXECVPE == 1))
        if (execvpe(exec->str, data, (char* const*)envp) == -1)
#       else
        if (execve(exec->str, data, (char* const*)envp) == -1)
#       endif
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_IPT_EXEC, exec->str, strerror(errno));
            errno = 0;
        }
        _exit(127);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPT_FORK, strerror(errno));
        errno = 0;
    }
    else
    {
        int stat;
        pid_t wpid;
        time_t start = (time(NULL) + 5000);
        do {
            if ((wpid = waitpid(p, &stat, WNOHANG)) == 0)
            {
                if (time(NULL) > start)
                {
                    kill(p, SIGKILL);
                    __GETTIME_EXEC(
                        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "ipfw_exec [%s]: killing pid -> %d", tms.str, p)
                    );
                    break;
                }
                sched_yield();
            }
        } while ((wpid == 0) && (time(NULL) <= start));

#       if defined(DEBUG_NODAEMON)
        if (WIFEXITED(stat))
        {
            __GETTIME_EXEC(
                __MACRO_TOSCREEN__(STR_IPFW_INFO "ipfw_exec [%s]: normal exit, status -> %d", tms.str, WEXITSTATUS(stat))
            );
        }
        else if (WIFSIGNALED(stat))
        {
            __GETTIME_EXEC(
                __MACRO_TOSCREEN__(STR_IPFW_ERROR "ipfw_exec [%s]: pid %d terminated, status -> %d", tms.str, p, WTERMSIG(stat))
            );
        }
#       endif
    }
}
FUNEXPORT void * ipfw_parse_line(string_s *sline)
{
    if (
        (sline == NULL) ||
        (sline->str == NULL) ||
        (sline->sz < 2) ||
        (sline->str[0] == '#') ||
        (sline->str[0] == '/') ||
        (sline->str[1] != '|')
       )
    {
        return NULL;
    }

    int idx;
    char *param[10] = { NULL }, *data[10] = { NULL };
    param[0] = execwrap.str;

    if ((idx = string_split((char * const)sline->str, (char**)data, __NELE(data))) <= 0)
    {
        return NULL;
    }
    for (int i = 0; i < __NELE(ipfwl); i++)
    {
        if (!__CHECK_ipfw_label_t(ipfwl[i], data[0], 1))
        {
            continue;
        }

#       if 0
        for (int i = 0; i < idx; i++)
        {
            printf("* ipfw_parse_line:\tloop (%d): %s\n",
                i,
                data[i]
            );
        }
#       endif

        if (idx != ipfwl[i].nele)
        {
            return NULL;
        }

#       if 0
            printf("* ipfw_parse_line:\tipfwl\n\t\t[%d] -> [%s]\n\t\t(%s)\n\t\t(%s)\n\t\t(%s)\n",
                i,
                ipfwl[i].name.str,
                ipfwl[i].nid[0].str,
                ipfwl[i].nid[1].str,
                ipfwl[i].nid[2].str
            );
#       endif

#       if (HAVE_IPFW == 1)
            param[1] = ipfwl[i].nid[1].str;
#       elif (HAVE_IPFW == 2)
            param[1] = ipfwl[i].nid[2].str;
#       elif (HAVE_IPFW == 3)
            param[1] = ipfwl[i].nid[1].str;
#       elif (HAVE_IPFW == 4)
            param[1] = ipfwl[i].nid[2].str;
#       elif (HAVE_IPFW == 5)
            param[1] = ipfwl[i].nid[0].str;
#       endif

        if(!ipfw_select_(ipfwl[i].label, (char **)param, (char **)data))
        {
            continue;
        }
        ipfwl[i].count++;
        ipfw_exec_(&execwrap, (char**)param);
    }
    return NULL;
}
FUNEXPORT void ipfw_exec(ipfw_label_e label, char **data)
{
    if (!__ipfw_checkbin())
    {
        return;
    }

    char *param[10] = { NULL };
    param[0] = execwrap.str;

    for (int i = 0; i < __NELE(ipfwl); i++)
    {
        if (ipfwl[i].label == label)
        {
#           if (HAVE_IPFW == 1)
               param[1] = ipfwl[i].nid[1].str;
#           elif (HAVE_IPFW == 2)
               param[1] = ipfwl[i].nid[2].str;
#           elif (HAVE_IPFW == 3)
               param[1] = ipfwl[i].nid[1].str;
#           elif (HAVE_IPFW == 4)
               param[1] = ipfwl[i].nid[2].str;
#           elif (HAVE_IPFW == 5)
               param[1] = ipfwl[i].nid[0].str;
#           endif

            if(ipfw_select_(ipfwl[i].label, (char **)param, data))
            {
                ipfw_exec_(&execwrap, (char**)param);
            }
            return;
        }
    }
}
FUNEXPORT ipfw_label_t * ipfw_stat_get(ipfw_label_e label)
{
    for (int i = 0; i < __NELE(ipfwl); i++)
    {
        if (ipfwl[i].label == label)
        {
            return &ipfwl[i];
        }
    }
    return NULL;
}
FUNEXPORT void ipfw_stat_prn(void)
{
    __MACRO_TOSCREEN__(STR_IPFW_INFO "%s:", "Staus block list");

    for (int i = 0; i < __NELE(ipfwl); i++)
    {
        if (ipfwl[i].count > 0)
        {
            __MACRO_TOSCREEN__("\t\t* %s list\t -> %lu", ipfwl[i].name.str, ipfwl[i].count);
        }
    }
}
FUNEXPORT void ipfw_parse(string_s fpath)
{
    if (!__ipfw_checkbin())
    {
        return;
    }
    if (!file_exists_fp(&fpath))
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "ipfw parse -> %s, %s",
            fpath.str,
            strerror(errno)
        );
        return;
    }
    ipfw_stat_clear_();
    file_read(&fpath, (fwall_cb)ipfw_parse_line);
}
