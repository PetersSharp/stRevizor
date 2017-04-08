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
#include "../../util/zip/xzip.h"

#define __XML_ARCHIVE_NAME "dump.xml"

#if defined(HAVE_EXPAT_H)
#  include <expat.h>
#else
#  error "Needed libexpat header: expat.h"
#endif

#if defined(HAVE_ICONV_H)
#  include <iconv.h>
#else
#  error "Needed iconv header: iconv.h"
#endif

extern int isrun;
extern string_s insource[];
extern int inprocess[];

xml_search_t xmlsection[] = {
    { XML_ELE_IP, { __ADD_string_s("ip") }},
    { XML_ELE_DNS,{ __ADD_string_s("domain") }},
    { XML_ELE_URL,{ __ADD_string_s("url") }},
    { XML_ELE_CNT,{ __ADD_string_s("content") }},
    { XML_ELE_REG,{ __ADD_string_s("reg:register") }}
};
xml_search_t xmlattr[] = {
    { XML_ATTR_IP,   { __ADD_string_s("ip") }},
    { XML_ATTR_DNS,  { __ADD_string_s("domain") }},
    { XML_ATTR_DNSM, { __ADD_string_s("domain-mask") }},
    { XML_ATTR_DEF,  { __ADD_string_s("default") }},
};
string_s regattr[] = {
    { __ADD_string_s("updateTimeUrgently") },
    { __ADD_string_s("blockType") }
};

static xml_attr_e find_attr(const char *label)
{
    xml_attr_e t = XML_ATTR_DEF;
    for (int i = 0; i < __NELE(xmlattr); i++)
    {
        if (memcmp(label, xmlattr[i].token.str, xmlattr[i].token.sz) == 0)
        {
            t = xmlattr[i].type;
        }
    }
    return t;
}
static xml_ele_e find_label(const char *label)
{
    xml_ele_e t = XML_ELE_NONE;
    for (int i = 0; i < __NELE(xmlsection); i++)
    {
        if (memcmp(label, xmlsection[i].token.str, xmlsection[i].token.sz) == 0)
        {
            t = xmlsection[i].type;
        }
    }
    return t;
}
static string_s ** alloc_xmlele_(string_s **eles, int *cnt, char *txt, int sz)
{
    int idx = (*cnt);

    do {
        if ((idx == 0) && (eles == NULL))
        {
            if ((eles = (string_s**)calloc((idx + 1), sizeof(string_s*))) == NULL) { break; }
        }
        else
        {
            if ((eles = (string_s**)realloc(eles, ((idx + 1) * sizeof(string_s*)))) == NULL) { break; }
        }
        if ((eles[idx] = calloc(1, sizeof(string_s))) == NULL) { break; }
        if ((eles[idx]->str = calloc((sz + 1), sizeof(char))) == NULL) { free(eles[idx]); break; }

        memcpy(eles[idx]->str, txt, sz);
        eles[idx]->sz = sz;
        eles[idx]->str[sz] = '\0';
        (*cnt)++;
        return eles;

    } while(0);

    return NULL;
}
static void correct_dot(string_s *ele)
{
    /* correct way .. stupid.. domain or url name end of dot */
    if (
        ((ele->sz - 1) > 0) &&
        (ele->str[(ele->sz - 1)] == '.')
       )
    {
        ele->sz--;
        ele->str[ele->sz] = '\0';
    }
}
static int alloc_xmlele(xml_ele_t *ele, char *txt, int sz)
{
    switch (ele->type)
    {
        case XML_ELE_IP:
        {
            if ((ele->ip  = alloc_xmlele_(ele->ip, &ele->cip, txt, sz)) == NULL) { return -1; }
            break;
        }
        case XML_ELE_DNS:
        {
            if ((ele->dns = alloc_xmlele_(ele->dns, &ele->cdns, txt, sz)) == NULL) { return -1; }
            correct_dot(ele->dns[(ele->cdns - 1)]);
            break;
        }
        case XML_ELE_URL:
        {
            if ((ele->url = alloc_xmlele_(ele->url, &ele->curl, txt, sz)) == NULL) { return -1; }
            correct_dot(ele->url[(ele->curl - 1)]);
            break;
        }
        default:
        {
            return 1;
        }
    }
    return 0;
}
static void free_xmlele_(string_s **eles, int cnt)
{
    if (eles == NULL)
    {
        return;
    }

    int i;

    for (i = 0; i < cnt; i++)
    {
        free(eles[i]->str);
        free(eles[i]);
    }
    free(eles);
}
static void free_xmlele(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }
    if ((ele->cip > 0) && (ele->ip != NULL))
    {
        free_xmlele_(ele->ip, ele->cip);
        ele->ip  = NULL;
    }
    if ((ele->cdns > 0) && (ele->dns != NULL))
    {
        free_xmlele_(ele->dns, ele->cdns);
        ele->dns  = NULL;
    }
    if ((ele->curl > 0) && (ele->url != NULL))
    {
        free_xmlele_(ele->url, ele->curl);
        ele->url  = NULL;
    }
    ele->cip     = 0;
    ele->cdns    = 0;
    ele->curl    = 0;
    ele->type    = XML_ELE_NONE;
}
static void free_xmlele_list(xml_ele_t *ele)
{
    if (ele == NULL)
    {
        return;
    }
    if (ele->fp != NULL)
    {
        fclose(ele->fp);
    }
    if (ele->squhttp != NULL)
    {
        fclose(ele->squhttp);
    }
    if (ele->squhttps != NULL)
    {
        fclose(ele->squhttps);
    }
    if (ele->sqphttp != NULL)
    {
        fclose(ele->sqphttp);
    }
    if (ele->sqphttps != NULL)
    {
        fclose(ele->sqphttps);
    }
    if ((ele->lip_ip.idx > 0) && (ele->lip_ip.arr != NULL))
    {
        arr_item_free(&ele->lip_ip);  ele->lip_ip.arr = NULL; ele->lip_ip.idx = 0;
    }
    if ((ele->lip_dns.idx > 0) && (ele->lip_dns.arr != NULL))
    {
        arr_item_free(&ele->lip_dns); ele->lip_dns.arr = NULL; ele->lip_dns.idx = 0;
    }
    if ((ele->lip_url.idx > 0) && (ele->lip_url.arr != NULL))
    {
        arr_item_free(&ele->lip_url); ele->lip_url.arr = NULL; ele->lip_url.idx = 0;
    }
    if ((ele->lip_bgp.idx > 0) && (ele->lip_bgp.arr != NULL))
    {
        arr_item_free(&ele->lip_bgp); ele->lip_bgp.arr = NULL; ele->lip_bgp.idx = 0;
    }
    if ((ele->lip_phttp.idx > 0) && (ele->lip_phttp.arr != NULL))
    {
        arr_item_free(&ele->lip_phttp); ele->lip_phttp.arr = NULL; ele->lip_phttp.idx = 0;
    }
    if ((ele->lip_phttps.idx > 0) && (ele->lip_phttps.arr != NULL))
    {
        arr_item_free(&ele->lip_phttps); ele->lip_phttps.arr = NULL; ele->lip_phttps.idx = 0;
    }
    if ((ele->lstr_uhttps.idx > 0) && (ele->lstr_uhttps.arr != NULL))
    {
        arr_item_free(&ele->lstr_uhttps); ele->lstr_uhttps.arr = NULL; ele->lstr_uhttps.idx = 0;
    }
    if ((ele->lstr_dns.idx > 0) && (ele->lstr_dns.arr != NULL))
    {
        arr_item_free(&ele->lstr_dns); ele->lstr_dns.arr = NULL; ele->lstr_dns.idx = 0;
    }
    if ((ele->lstr_url.idx > 0) && (ele->lstr_url.arr != NULL))
    {
        arr_item_free(&ele->lstr_url); ele->lstr_url.arr = NULL; ele->lstr_url.idx = 0;
    }
    if ((ele->lstr_host.idx > 0) && (ele->lstr_host.arr != NULL))
    {
        arr_item_free(&ele->lstr_host); ele->lstr_host.arr = NULL; ele->lstr_host.idx = 0;
    }
}
static void XMLCALL parse_ele(void *edata, const XML_Char *s, int sz)
{
    if ((sz == 0) || (edata == NULL) || (s == NULL))
    {
        return;
    }
    xml_ele_t *ele = (xml_ele_t*)edata;
    switch (alloc_xmlele(ele, (char*)s, sz))
    {
        case -1:
        {
            __MACRO_TOSYSLOG__(LOG_ERR,
                STR_XML_ERROR "-> XML alloc element error: %d",
                ele->type
            );
            break;
        }
        default:
        {
            break;
        }
    }
}
static void XMLCALL start_ele(void *edata, const char *name, const char **attr)
{
    if ((edata == NULL) || (name == NULL))
    {
        return;
    }
    xml_ele_t *ele = (xml_ele_t*)edata;
    ele->type = find_label(name);

    switch (ele->type)
    {
        case XML_ELE_REG:
        {
            for (int i = 0; attr[i]; i += 2)
            {
                if (memcmp((char*)attr[i], regattr[0].str, regattr[0].sz) == 0)
                {
                    __BUILD_string_static(fpath, HAVE_WATCH_BACKUP "/" TFTP_FILE_UPDATE);
                    string_s data = { (char*)attr[(i + 1)], strlen(attr[(i + 1)]) };
                    file_delete_fp(&fpath);
                    file_create_fp(&fpath, &data);
                }
            }
            ele->type = XML_ELE_NONE;
            break;
        }
        case XML_ELE_CNT:
        {
            ele->attr = XML_ATTR_DEF;
            for (int i = 0; attr[i]; i += 2)
            {
                if (memcmp((char*)attr[i], regattr[1].str, regattr[1].sz) == 0)
                {
                    ele->attr = find_attr(attr[(i + 1)]);
                }
            }
            ele->type = XML_ELE_NONE;
            break;
        }
        default:
        {
            break;
        }
    }
}
static void XMLCALL end_ele(void *edata, const char *name)
{
    if ((edata == NULL) || (name == NULL))
    {
        return;
    }
    xml_ele_t *ele = (xml_ele_t*)edata;

    if (find_label(name) == XML_ELE_CNT)
    {
        plist_data(ele);
        free_xmlele(ele);
    }
}

static void * plist_run(void *vpath)
{
    FILE *fp = NULL;
    char rbuf[(BUFSIZ * 2)],
         obuf[((BUFSIZ * 2) * sizeof(wchar_t) + 1)],
        *rb, *ob;

    xml_path_t *xpath = (xml_path_t*)vpath;

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    int i, end, ret = 0;
    iconv_t cnv;
    xml_ele_t edata;
    XML_Parser xmlp = NULL;

    memset(&edata, 0, sizeof(xml_ele_t));

    if ((fp = fopen(xpath->spath->str, "r")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open read error: %s/%s",
            xpath->spath->str,
            strerror(errno)
        );
        __PTH_PARSER_EXIT(NULL, NULL, NULL, -1, NULL);
    }
    if ((edata.fp = fopen(xpath->opath->str, "w")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open write error: %s - %s",
            xpath->opath->str,
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, NULL);
    }

#   if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
    if ((edata.squhttp = fopen(HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTP, "w")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open write error: " HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTP " - %s",
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, NULL);
    }
    if ((edata.squhttps = fopen(HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTPS, "w")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open write error: " HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_HTTPS " - %s",
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, NULL);
    }
    if ((edata.sqphttp = fopen(HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTP, "w")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open write error: " HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTP " - %s",
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, NULL);
    }
    if ((edata.sqphttps = fopen(HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTPS, "w")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> File open write error: " HAVE_WATCH_BACKUP "/" TFTP_FILE_SQUID_PHTTPS " - %s",
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, NULL);
    }
#   else
    edata.squhttp   = NULL;
    edata.squhttps  = NULL;
    edata.sqphttp  = NULL;
    edata.sqphttps = NULL;
#   endif

    if ((cnv = iconv_open((const char*)&"UTF-8", (const char*)&"WINDOWS-1251")) == (iconv_t)-1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> Open iconv: %s",
            strerror(errno)
        );
        __PTH_PARSER_EXIT(fp, NULL, NULL, -1, &edata);
    }
    if ((xmlp = XML_ParserCreate((const XML_Char*)&"UTF-8")) == NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_XML_ERROR "-> Parser create: %s",
            strerror(errno)
        );

        __PTH_PARSER_EXIT(fp, cnv, NULL, -1, &edata);
    }

    XML_SetUserData(xmlp, &edata);
    XML_SetElementHandler(xmlp, start_ele, end_ele);
    XML_SetCharacterDataHandler(xmlp, parse_ele);

    do {

        rb = (char*)&rbuf;
        ob = (char*)&obuf;
        size_t olen = (size_t)sizeof(obuf),
               rlen = (size_t)fread(rbuf, 1, sizeof(rbuf), fp);

        end = (int)(rlen < sizeof(rbuf));

        for (i = (rlen - 1); i >= 1; i--)
        {
            if ((rbuf[i] == '>') && (i >= __CSZ(__XML_ELE_TOKEN)))
            {
                if (memcmp(rb + (i - __CSZ(__XML_ELE_TOKEN) + 1), CP __XML_ELE_TOKEN, __CSZ(__XML_ELE_TOKEN)) == 0)
                {
                    fseek(fp, -(rlen - i - 1), SEEK_CUR);
                    rlen = (i + 1);
                    end  = 0;
                    break;
                }
            }
        }
        if (iconv(cnv, (char**)&rb, &rlen, (char**)&ob, &olen) == (size_t)-1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR,
                STR_XML_ERROR "-> Iconv parse: %s",
                strerror(errno)
            );
            ret = -1;
            break;
        }
        if (XML_Parse(xmlp, obuf, (sizeof(obuf) - olen), end) == XML_STATUS_ERROR)
        {
            __MACRO_TOSYSLOG__(LOG_ERR,
                STR_XML_ERROR "-> XML parse: %s, line %" XML_FMT_INT_MOD "u",
                XML_ErrorString(XML_GetErrorCode(xmlp)),
                XML_GetCurrentLineNumber(xmlp)
            );
            ret = -1;
            break;
        }

    } while ((!end) && (isrun));

    if ((isrun) && (!ret))
    {
        plist_white_list(&edata);
        plist_fixed_list(&edata);
#       if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
        plist_squid_port(&edata);
#       endif

#       if (defined(HAVE_XML_PARSER_DEBUGA) && (HAVE_XML_PARSER_DEBUGA == 1))
/*
        plist_result_prn(&edata);
*/
#       endif
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    __PTH_PARSER_EXIT(fp, cnv, xmlp, ((isrun) ? ret : -1), &edata);
}

int plist_xml_prog(string_s *fdir, const string_s *sname, const string_s *oname)
{
    __BUFFER_CREATE__(spath);
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(spath, -1, "%s/%s", fdir->str, sname->str);
    __MACRO_SNPRINTF__(opath, -1, "%s/%s", fdir->str, oname->str);
    xml_path_t xpath = { &spath, &opath };

    do {
        int ret;
        pthread_t t1;
        if (pthread_create(&t1, NULL, plist_run, (void*)&xpath) != 0) { break; }
        if (pthread_join(t1,(void**)&ret) != 0) { break; }
        return ret;

    } while(0);

    if (file_exists_fp(&opath))
    {
        file_delete_fp(&opath);
    }
    return -1;
}
int plist_zip_prog(string_s *fdir, const string_s *sname, const string_s *oname, const string_s *bname)
{
    __BUFFER_CREATE__(spath);
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(spath, -1, "%s/%s", fdir->str, sname->str);
    __MACRO_SNPRINTF__(opath, -1, "%s/%s", fdir->str, oname->str);

    struct zip_t *rzip = zip_open(spath.str, 0, 'r');
    {
        zip_entry_open(rzip, CP __XML_ARCHIVE_NAME);
        {
            zip_entry_fread(rzip, opath.str);
        }
        zip_entry_close(rzip);
    }
    zip_close(rzip);

    return plist_xml_prog(fdir, oname, bname);
}
