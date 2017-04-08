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
#include "filter/st-revizor-lock.h"
#include "filter/ipfw/st-revizor-ipfw-list.h"

extern const string_s insource[];
extern int inprocess[];

static int isbusy = 0;

FUNEXPORT void watch_dir_setbusy(void)
{
    __SYNC_MEM(isbusy = ((isbusy) ? 0 : 1));
}
FUNEXPORT int watch_dir_checkin(string_s fpath, watch_file_e idx)
{
    do {
        if (isbusy) { break; }
        if (file_exists(&fpath, (string_s*)&insource[idx])    == 0) { break; }
        if (file_exists(&fpath, (string_s*)&insource[IN_FILE_COMPLETE]) == 0) { break; }
        if (file_exists(&fpath, (string_s*)&insource[IN_FILE_PROCESS])  == 1) { break; }
        return 1;

    } while(0);
    return 0;
}
static int __watch_read_stat(string_s *fupstat, char **param, int cnt)
{
    if (file_exists_fp(fupstat))
    {
        int idx = -1;
        __BUFFER_CREATE__(supstat);
        __cleanup_new_fclose(FILE) fp = NULL;
        do {
            if ((fp = fopen(fupstat->str, "r")) == NULL) { break; }
            if ((supstat.sz = fread(supstat.str, sizeof(char), (__BUFFER_SIZE__ - 1), fp)) <= 0) { break; }
            supstat.str[supstat.sz] = '\0';
            if (supstat.sz > 0)
            {
                if ((idx = string_split((char * const)supstat.str, param, cnt)) < (cnt - 1)) { break; }
            }
            return idx;

        } while(0);
        __cleanup_notsupport_p(auto_fcloseFILE,fp);
    }
    return -1;
}
FUNEXPORT void watch_dir_autoconfig(string_s *fdir)
{

#if ( \
    (!defined(HAVE_WATCH_XML_AUTO_ENABLE) || (HAVE_WATCH_XML_AUTO_ENABLE == 0)) && \
    (!defined(HAVE_WATCH_JSON_AUTO_ENABLE) || (HAVE_WATCH_JSON_AUTO_ENABLE == 0)))

#       warning "no autoconfig format selected"

#else

    int ipfw, ipt, ips, dnsfilter, urlfilter, xmlparser, xmlzip;
    unsigned long dnshost, dnswild, urlport, urlhost, urluri;
        ipfw = ipt = ips = dnsfilter = urlfilter = xmlparser = xmlzip = 0;
        dnshost = dnswild = urlport = urlhost = urluri = 0UL;
    static const char *netfiltred, *dnsfiltred, *urlfiltred;

#   if (defined(HAVE_IPFW_NET) && defined(HAVE_IPFW_ENABLE))
    netfiltred = CP HAVE_IPFW_NET;
#   else
    netfiltred = CP "- none -";
#   endif

#   if (defined(HAVE_DNS_FILTRED) && defined(HAVE_DNS_FILTER_ENABLE))
    dnsfiltred = CP HAVE_DNS_FILTRED;
#   else
    dnsfiltred = CP "- none -";
#   endif

#   if (defined(HAVE_HTTP_FILTRED) && defined(HAVE_HTTP_FILTER_ENABLE))
    urlfiltred = CP HAVE_HTTP_FILTRED;
#   else
    urlfiltred = CP "- none -";
#   endif

#   if defined(HAVE_IPFW_ENABLE)
    ipfw  = HAVE_IPFW_ENABLE;
#   endif

#   if defined(HAVE_IPT_ENABLE)
    ipt   = HAVE_IPT_ENABLE;
#   endif

#   if defined(HAVE_IPS_ENABLE)
    ips   = HAVE_IPS_ENABLE;
#   endif

#   if defined(HAVE_DNS_FILTER_ENABLE)
    dnsfilter = HAVE_DNS_FILTER_ENABLE;
    {
        char *param[2] = { NULL };
        __BUILD_string_static(fupstat, HAVE_WATCH_BACKUP "/" TFTP_FILE_STATDNS);
        if (__watch_read_stat(&fupstat, (char**)param, __NELE(param)) != -1)
        {
            dnshost = strtoul(param[0], NULL, 10);
            dnswild = strtoul(param[1], NULL, 10);
        }
    }
#   endif

#   if defined(HAVE_HTTP_FILTER_ENABLE)
    urlfilter = HAVE_HTTP_FILTER_ENABLE;
    {
        char *param[3] = { NULL };
        __BUILD_string_static(fupstat, HAVE_WATCH_BACKUP "/" TFTP_FILE_STATURL);
        if (__watch_read_stat(&fupstat, (char**)param, __NELE(param)) != -1)
        {
            urlport = strtoul(param[0], NULL, 10);
            urlhost = strtoul(param[1], NULL, 10);
            urluri  = strtoul(param[2], NULL, 10);
        }
    }
#   endif

#   if defined(HAVE_XML_PARSER_ENABLE)
    xmlparser = HAVE_HTTP_FILTER_ENABLE;
    xmlzip = 1;
#   endif

    __BUILD_string_static(fupddate, HAVE_WATCH_BACKUP "/" TFTP_FILE_UPDATE);
    __BUFFER_CREATE__(supddate);

    if (file_exists_fp(&fupddate))
    {
        __cleanup_new_fclose(FILE) fp = NULL;
        do {
            if ((fp = fopen(fupddate.str, "r")) == NULL) { break; }
            if ((supddate.sz = fread(supddate.str, sizeof(char), (__BUFFER_SIZE__ - 1), fp)) <= 0) { break; }
            supddate.str[supddate.sz] = '\0';
        } while(0);
        __cleanup_notsupport_p(auto_fcloseFILE,fp);
        if (supddate.sz == 0) { supddate.str = CP ""; }
    }

    __BUFFER_CREATE_SIZE__(setup, 1024);
    __BUILD_string_static(fname, "");

#   if (defined(HAVE_WATCH_JSON_AUTO_ENABLE) && (HAVE_WATCH_JSON_AUTO_ENABLE == 1))
    __MACRO_SNPRINTF_SIZE__(setup, , 1024,
        "{"                   \
        "\"ipfw\":%d,"        \
        "\"ipt\":%d,"         \
        "\"ips\":%d,"         \
        "\"filter\":\"%s\","    \
        "\"dnsfilter\":{\"enable\":%d,\"host\":%lu,\"wild\":%lu,\"filter\":\"%s\"},"   \
        "\"urlfilter\":{\"enable\":%d,\"port\":%lu,\"host\":%lu,\"uri\":%lu,\"filter\":\"%s\"},"   \
        "\"xmlparser\":{\"enable\":%d,\"zip\":%d},"   \
        "\"datelist\":\"%s\"" \
        "}",
        ipfw, ipt, ips, netfiltred,
            dnsfilter, dnshost, dnswild, dnsfiltred,
            urlfilter, urlport, urlhost, urluri, urlfiltred,
            xmlparser, xmlzip,
        supddate.str
    );
    __SET_string_s(fname, TFTP_FILE_AUTOCONF ".json");
    file_delete(fdir, (string_s*)&fname);
    file_create(fdir, (string_s*)&fname, &setup);
#   endif

#   if (defined(HAVE_WATCH_XML_AUTO_ENABLE) && (HAVE_WATCH_XML_AUTO_ENABLE == 1))
    __MACRO_SNPRINTF_SIZE__(setup, , 1024,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
        "<root>\n" \
        "  <ipfw>%d</ipfw>\n" \
        "  <ipt>%d</ipt>\n" \
        "  <ips>%d</ips>\n" \
        "  <filter>%s</filter>\n" \
        "  <dnsfilter>\n" \
        "    <enable>%d</enable>\n" \
        "    <host>%lu</host>\n" \
        "    <wild>%lu</wild>\n" \
        "    <filter>%s</filter>\n" \
        "  </dnsfilter>\n" \
        "  <urlfilter>\n" \
        "    <enable>%d</enable>\n" \
        "    <port>%lu</port>\n" \
        "    <host>%lu</host>\n" \
        "    <uri>%lu</uri>\n" \
        "    <filter>%s</filter>\n" \
        "  </urlfilter>\n" \
        "  <xmlparser>\n" \
        "    <enable>%d</enable>\n" \
        "    <zip>%d</zip>\n" \
        "  </xmlparser>\n" \
        "  <datelist>%s</datelist>\n" \
        "</root>\n",
        ipfw, ipt, ips, netfiltred,
            dnsfilter, dnshost, dnswild, dnsfiltred,
            urlfilter, urlport, urlhost, urluri, urlfiltred,
            xmlparser, xmlzip,
        supddate.str
    );
    __SET_string_s(fname, TFTP_FILE_AUTOCONF ".xml");
    file_delete(fdir, (string_s*)&fname);
    file_create(fdir, (string_s*)&fname, &setup);
#   endif

#endif
}
