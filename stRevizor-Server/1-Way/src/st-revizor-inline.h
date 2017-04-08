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

#ifndef REVIZOR_INLINE_H
#define REVIZOR_INLINE_H

#include "st-revizor-watch.h"

#if defined(HAVE_DEBUGSYM) && (HAVE_DEBUGSYM == 1)
#   define __MACRO_DEBUG_PRN_(B, A) \
        printf("\t*DBG ipt:[%s] ips:[%s] - [%s]\n",B.ipt.str, B.ips.str, A.str)
#else
#   define __MACRO_DEBUG_PRN_(B, A)
#endif

/* Add rule to fo fwall */
#define __MACRO_IPTRULE__(B, A, X, ...)       \
    __MACRO_SNPRINTF__(A, X, __VA_ARGS__); \
    __MACRO_DEBUG_PRN_(B, A); \
    fwall_exec(B, A);           \
    memset(A.str, '\0', __BUFFER_SIZE__)

/* Check bash/sh vulnerability string */
#define __MACRO_VULNERABILITY__(A) \
    if (shell_vulnerability(A.str) == 1) { continue; }

FUNINLINE int shell_vulnerability(char * const a)
{
    if (strchr(a, '`') != NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Vulnerability]: broken string: [%s]", a);
        return 1;
    }
    return 0;
}
FUNINLINE int string_isspace(char const a)
{
    return (((a == 0x20) || (a == 0x0d) || (a == 0x0a)) ? 1 : 0);
}
FUNINLINE void string_trim(string_s *src)
{
    src->sz = 0;
    char *p = src->str, *q = src->str;
    while (string_isspace(*q))            { ++q; }
    while (*q)                            { *p++ = *q++; src->sz++; }
    *p = '\0';
    while (p > src->str && string_isspace(*--p)) { *p = '\0'; src->sz--; }
}
FUNINLINE void string_case(string_s *src, int sel)
{
    for (char *p = src->str; *p; ++p) {
        *p = ((!sel) ?
                (((*p > 0x40) && (*p < 0x5b)) ? (*p | 0x60) : *p) :
                (((*p > 0x60) && (*p < 0x7b)) ? (*p - 0x20) : *p)
        );
    }
}
FUNINLINE fwall_pip_t string_split(char * const a)
{
    fwall_pip_t pip = {0,{0},{0}};
    char *t = NULL;
    while ((t = strsep((char**)&a, "|")) != NULL)
    {
        pip.cnt++;
        if (pip.cnt == 1)
        {
            pip.ip.str = strdup(t);
            continue;
        }
        if (pip.cnt == 2)
        {
            pip.port.str = strdup(t);
        }
        return pip;
    }
    return pip;
}
FUNINLINE void file_read(string_s fpath, nfbin_t bpath, fwall_cb fcb)
{
    int cnt = 0;
    FILE *fp;
    string_s data = {0};

    if ((fp = fopen(fpath.str, "r")) != NULL)
    {
        while (getline(&data.str, ((size_t*)&data.sz), fp) != -1)
        {
            string_trim(&data);
            __MACRO_VULNERABILITY__(data);
            fcb(bpath, data);
            cnt++;
        }
        if (data.str != NULL)
        {
            free(data.str);
        }
        fclose(fp);
        __MACRO_TOSCREEN__("\tEnd, count address: %d -> %s", cnt, fpath.str);
    }
}
FUNINLINE void fwall_exec(nfbin_t bpath, string_s data)
{
    pid_t p = fork();
    if (!p)
    {
        if (execlp(bpath.bin, bpath.bin, bpath.ipt.str, bpath.ips.str, data.str, (char *)0) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: exec '%s': %s", bpath.bin, strerror(errno));
            errno = 0;
        }
        _exit(127);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: exec fork: %s", strerror(errno));
        errno = 0;
    }
    else
    {
        int stat;
        waitpid(p, &stat, 0);
    }
}

#endif
