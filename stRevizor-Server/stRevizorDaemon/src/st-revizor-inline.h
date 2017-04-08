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

/* Check bash/sh vulnerability string */
#define __MACRO_VULNERABILITY__(A) \
    if (shell_vulnerability(A.str) == 1) { continue; }


#define __build_local_xtoa_func(name, type)                            \
    FUNINLINE int __EV(string_,name)(type x, char str []) {            \
        int d = 0, i = 0, n = 0, neg = ((x < 0) ? 1 : 0); char c1, c2; \
        if (!x)  { str[i++] = '0'; str[i] = '\0'; return 1; }          \
        if (neg) { x = (0 - x); }                                      \
        while(x) {                                                     \
            __builtin_prefetch(&str[(i+2)]);                           \
            str[i++] = (x % 10) + '0'; x = x / 10;                     \
        }                                                              \
        if (neg) { str[i++] = '-'; }                                   \
        str[i] = '\0';                                                 \
        n = ((i > 0) ? (i - 1) : 0);                                   \
        while((n > 0) && (n > (int)((i - 1) / 2))) {                   \
            c1 = str[n]; c2 = str[d];                                  \
            str[n--] = c2; str[d++] = c1;                              \
        }                                                              \
        return i;                                                      \
    }

__build_local_xtoa_func(itoa,  int)
__build_local_xtoa_func(ltoa,  long)
__build_local_xtoa_func(ultoa, unsigned long)

FUNINLINE int shell_vulnerability(char * const a)
{
    if (strchr(a, '`') != NULL)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPT_CHECK, a);
        return 1;
    }
    return 0;
}
FUNINLINE int string_isspace(char const a)
{
    return (((a == 0x20) || (a == 0x0d) || (a == 0x0a)) ? 1 : 0);
}
FUNINLINE int string_isdigit(string_s *src)
{
    for (char *p = src->str; *p; ++p) {
        if ((*p < '0') || (*p > '9')) { return 0; }
    }
    return 1;
}
FUNINLINE char * string_char_substring(string_s *src, string_s *dst, char c, int cnt)
{
    int chr = 1, idx = src->sz;

    while (idx > 0)
    {
        if (src->str[idx] == c)
        {
            if (cnt == (chr++))
            {
                dst->sz = (src->sz - idx - 1);
                dst->str = (char*)(src->str + idx + 1);
                return dst->str;
            }
        }
        idx--;
    }
    return NULL;
}
FUNINLINE int string_poschar(char *str, char c)
{
    int pos  = 0;
    char *p  = str;
    while (*p == c) { ++pos; ++p; }
    return ((pos == 0) ? 0 : (pos - 1));
}
FUNINLINE int string_poschar_s(string_s *src, char c)
{
    return string_poschar(src->str, c);
}
FUNINLINE void string_skipchar(string_s *src, char c)
{
    int pos  = 0;
    char *p  = src->str;
    while (*p == c) { ++pos; ++p; }
    pos = ((pos == 0) ? 0 : (pos - 1));
    src->str = (char*)(src->str + pos);
    src->sz  = (((src->sz > 0) && ((src->sz - pos) > 0)) ? (src->sz - pos) : strlen(src->str));
}
FUNINLINE void string_trim(string_s *src)
{
    src->sz = 0;
    char *p, *q;
    p = q = src->str;
    while (string_isspace(*q))                       { ++q; }
    while (*q)                                       { *p++ = *q++; src->sz++; } *p = '\0';
    while ((p > src->str) && (string_isspace(*--p))) { *p = '\0'; src->sz--; }
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
FUNINLINE int string_split(char * const a, char **arr, int arrn)
{
    int idx = 0;
    char **str = (char**)&a;
    while (((arr[idx++] = strsep(str, "|")) != NULL) && (idx < arrn))
#   if 0
    {
        printf("\t* DEBUG string_split =\t[%s][%d] <-> (%s)(%d)\n", arr[(idx - 1)], idx, *str, arrn);
    }
#   else
    ;
#   endif
    return ((idx > 1) ? (idx - 1) : 0);
}

FUNINLINE fwall_pip_t string_split_(char * const a)
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
FUNINLINE int console_set(void)
{
#   if defined(DEBUG_SYSLOG)
    int line = 0;
#   endif

    do {
        int io;

        if ((io = open(CONSREDIRECT, O_RDONLY)) == -1)
        {
#           if defined(DEBUG_SYSLOG)
            line = __LINE__;
#           endif
            break;
        }
        dup2(io, STDIN_FILENO); close(io);
        if ((io = open(CONSREDIRECT, O_WRONLY | O_CREAT | O_APPEND)) == -1)
        {
#           if defined(DEBUG_SYSLOG)
            line = __LINE__;
#           endif
            break;
        }
        dup2(io, STDOUT_FILENO); close(io);
        if ((io = open(CONSREDIRECT, O_WRONLY | O_CREAT | O_APPEND)) == -1)
        {
#           if defined(DEBUG_SYSLOG)
            line = __LINE__;
#           endif
            break;
        }
        dup2(io, STDERR_FILENO); close(io);
        if ((io = open(CONSREDIRECT, O_RDWR)) == -1)
        {
#           if defined(DEBUG_SYSLOG)
            line = __LINE__;
#           endif
            break;
        }
        if (fcntl(io, F_SETFD, FD_CLOEXEC) == -1)
        {
#           if defined(DEBUG_SYSLOG)
            line = __LINE__;
#           endif
            break;
        }
        setvbuf(fdopen(io,"r"), NULL, _IONBF, 0);

        return 0;

    } while(0);

    __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "console set error in line:%d -> %s", line, strerror(errno));
    return -1;
}
FUNINLINE void proc_name_set(string_s *name)
{
#   if defined(HAVE_SYS_PRCTL_H)
#      include <sys/prctl.h>
#      if defined(PR_SET_NAME)
        __BUFFER_CREATE__(procstr);
        if (name->sz  > -1)
        {
            __MACRO_SNPRINTF__(procstr, , "revizor-%s-%d", name->str, name->sz);
        }
        else
        {
            __MACRO_SNPRINTF__(procstr, , "revizor-%s", name->str);
        }
        prctl(PR_SET_NAME, procstr.str);
#      endif
#   endif
}

#endif
