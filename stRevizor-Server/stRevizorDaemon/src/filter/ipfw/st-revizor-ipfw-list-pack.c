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
#include "st-revizor-ipfw-list-pack.h"

#include <sched.h>
#include <time.h>

extern int          ipfws;
extern ipfw_label_t ipfwl[];
extern string_s execwrap;

static const char *scrdir[] = {
    "ipfw_none",
    "ipfw_ipt",
    "ipfw_ips"
};

static void __ipfw_pack_context_(string_s *context, ipfw_label_e label)
{

    switch(label)
    {

#if (defined(HAVE_IPFW_ENABLE) && (HAVE_IPFW_ENABLE == 1))
#  if ((HAVE_IPFW_IPS_ENABLE) && (HAVE_IPFW_IPS_ENABLE == 1))
#       define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_CASE(B,ips)
#  elif ((HAVE_IPFW_IPT_ENABLE) && (HAVE_IPFW_IPT_ENABLE == 1))
#       define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_CASE(B,ipt)
#  else
#       define __IPFW_LABEL(A,B,C,D) __IPFW_LABEL_CASE(B,none)
#  endif
#       include "st-revizor-ipfw-list-define.h"
#       undef __IPFW_LABEL
#endif

        case IPFW_st_revizor_ipfw_script_s:
        {
            context->str = CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_s_tobin_start;
            context->sz  = (int)(
                CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_s_tobin_end -
                CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_s_tobin_start);
            break;
        }
        case IPFW_st_revizor_ipfw_script_e:
        {
            context->str = CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_e_tobin_start;
            context->sz  = (int)(
                CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_e_tobin_end -
                CP _binary_______src_filter_ipfw_script_st_revizor_ipfw_script_e_tobin_start);
            break;
        }
        default:
        {
            context->str = CP __IPFW_SCRSUB_TEMPLATE;
            context->sz  = (sizeof(__IPFW_SCRSUB_TEMPLATE) - 1);
            break;
        }
    }
}

void ipfw_pack_check(void)
{
    int i, n;
    string_s context = {0};

    __BUFFER_CREATE__(dirchk);
    __BUFFER_CREATE__(filechk);
    __BUFFER_CREATE__(addscr);
    __cleanup_new_fclose(FILE) fp = NULL;

    if (access(HAVE_WATCH_DRIVER "/exec", F_OK) == -1)
    {
        if (mkdir(HAVE_WATCH_DRIVER "/exec", 0775) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Unable create ipfw script directory %s/exec, %s.. Exit",
                HAVE_WATCH_DRIVER,
                strerror(errno)
            );
            exit(2);
        }
    }
    if (access(execwrap.str, F_OK) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Execute ipfw script %s, %s.. Create",
            execwrap.str,
            strerror(errno)
        );
        errno = 0;
        if ((fp = fopen(execwrap.str, "w")) == NULL)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Error write ipfw script %s, %s.. Exit",
                execwrap.str,
                strerror(errno)
            );
            exit(2);
        }
        __ipfw_pack_context_(&context, IPFW_st_revizor_ipfw_script_s);
        fwrite(context.str, context.sz, sizeof(char), fp);
    }
    do {
#       if (defined(HAVE_IPFW_ENABLE) && (HAVE_IPFW_ENABLE == 1))
#            if ((HAVE_IPFW_IPS_ENABLE) && (HAVE_IPFW_IPS_ENABLE == 1))
                i = 2;
#            elif ((HAVE_IPFW_IPT_ENABLE) && (HAVE_IPFW_IPT_ENABLE == 1))
                i = 1;
#            else
                i = 0;
#            endif
#       endif
        __MACRO_SNPRINTF__(dirchk, , HAVE_WATCH_DRIVER "/exec/%s", scrdir[i]);
        if (access(dirchk.str, F_OK) == -1)
        {
            if (mkdir(dirchk.str, 0775) == -1)
            {
                __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Unable create ipfw sub script directory %s -> %s",
                    dirchk.str,
                    strerror(errno)
                );
                __cleanup_notsupport_p(auto_fcloseFILE,fp);
                exit(2);
            }
            __MACRO_TOSCREEN__(STR_IPFW_ERROR "Create script directory: %s", dirchk.str);
        }
        for (n = 0; n < ipfws; n++)
        {
            __MACRO_SNPRINTF__(filechk, ,"%s/ipfw_%s.inc", dirchk.str, ipfwl[n].name.str);
            if (access(filechk.str, F_OK) == -1)
            {

                __ipfw_pack_context_(&context, ipfwl[n].label);
                file_create_fp(&filechk, &context);
                __MACRO_TOSCREEN__(STR_IPFW_ERROR "Create default script template: %s", filechk.str);
            }
            if (fp != NULL)
            {
                __MACRO_SNPRINTF__(addscr, ,__IPFW_SCR_CASEPATH, ipfwl[n].nid[i].str, filechk.str);
                fwrite(addscr.str, addscr.sz, sizeof(char), fp);
            }
        }

    } while(0);

    if (fp != NULL)
    {
        __ipfw_pack_context_(&context, IPFW_st_revizor_ipfw_script_e);
        fwrite(context.str, context.sz, sizeof(char), fp);

        if (chmod(execwrap.str, (S_IXUSR | S_IRUSR | S_IWUSR)) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Error change mode ipfw script %s, %s.. Exit",
                execwrap.str,
                strerror(errno)
            );
            __cleanup_notsupport_p(auto_fcloseFILE,fp);
            exit(2);
        }
        __cleanup_notsupport_p(auto_fcloseFILE,fp);
    }
}
