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

#ifndef REVIZOR_WATCH_H
#define REVIZOR_WATCH_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#if defined(HAVE_LIMITS_H)
#  include <limits.h>
#else
//#  error "Needed source header: limits.h"
#endif

#if defined(HAVE_SIGNAL_H)
#  include <signal.h>
#else
//#  error "Needed source header: signal.h"
#endif

#if defined(HAVE_PTHREAD_H)
#  include <pthread.h>
#else
//#  error "Needed source thread header: pthread.h"
#endif

#include "st-revizor-watch-debug.h"

#if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
#   include <syslog.h>
#endif

#include "./util/xbuild.h"
#include "./util/xmem.h"
#include "st-revizor-lang.h"

#define PROCID "revizor-watch"
#define SQPROCID "revizor-squid"
#define CONSREDIRECT "/dev/null"

#define TFTP_FILE_COMPLETE     "blocklist.complete"
#define TFTP_FILE_PROCESS      "blocklist.proccess"
#define TFTP_FILE_BTIMER       "blocklist.timer"
#define TFTP_FILE_BLXML        "dump.xml"
#define TFTP_FILE_BLZIP        "register.zip"
#define TFTP_FILE_BLIST        "blocklist.fw"
#define TFTP_FILE_PLIST        "blockplist.fw"
#define TFTP_FILE_UPDATE       "blocklist.date"
#define TFTP_FILE_AUTOCONF     "autoconfig"
#define TFTP_FILE_SQUID_HTTP   "squiduhttp.fw"
#define TFTP_FILE_SQUID_HTTPS  "squiduhttps.fw"
#define TFTP_FILE_SQUID_PHTTP  "squidphttp.fw"
#define TFTP_FILE_SQUID_PHTTPS "squidphttps.fw"

#define TFTP_FILE_STATDNS      "blockdns.stat"
#define TFTP_FILE_STATURL      "blockurl.stat"
#define TFTP_FILE_STATIPFW     "blockipfw.stat"

#define __EVALUTE_FUNC_CONCAT_(A,B) A ## B
#define __EVALUTE_FUNC_CONCAT(A,B)  __EVALUTE_FUNC_CONCAT_(A,B)

#define __EVALUTE_TO_STR_(A) #A
#define __EVALUTE_TO_STR(A) __EVALUTE_TO_STR_(A)

#define __EVALUTE_FUNC_ARGS_(_11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N
#define __EVALUTE_FUNC_ARGS(...) __EVALUTE_FUNC_ARGS_(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define __STATIC_CONST_SIZE(A) (int const) (sizeof(A)-1)

#define __NELE(A)      (sizeof(A) / sizeof(A[0]))
#define __EV(A,B)      __EVALUTE_FUNC_CONCAT(A,B)
#define __EVS(A)       __EVALUTE_TO_STR(A)
#define __CSZ(A)       __STATIC_CONST_SIZE(A)
#define __NOUSED       __attribute__((unused))
#define CP (char*)&

#define __BUFFER_SIZE__ 512

#if defined(__X_BUILD_ASM86)
#   define __label_set(A) A: __asm__ ("nop" ::: "memory")
#else
#   define __label_set(A) A:   
#endif

#if defined(__GNUC__)
#   define ELF_SECTION_ATR(A)  __attribute__ ((section(A)))
#else
#   define ELF_SECTION_ATR(A)
#endif

#if defined(__GNUC__) || defined(__GCC__)
#    define FUNALIAS(A) __attribute__((weak,alias(__EVS(A))))
#      if defined(__GCC__) || (defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__) && !defined(__GNUC_GNU_INLINE__))
#         define FUNINLINE static inline __attribute__((always_inline))
#      else
#         define FUNINLINE static inline __attribute__((always_inline))
#      endif
#    define FUNEXPORT // error if next gcc: __attribute__((visibility("default")))
#else
#    define FUNALIAS(A)
#    define FUNINLINE static inline
#    define FUNEXPORT
#endif

#if defined(_GNU_SOURCE)
#   define __CHECKARG(err,what) if (unlikely(what)) { return err; }
#else
#   define __CHECKARG(err,what)
#endif

#define __MACRO_SNPRINTF_OVERFLOW(A,B) \
    __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_BUF_ERROR "Buffer overflow size: %d -> %d, %s:%d", \
        (A), \
        (B), \
        __FILE__, \
        __LINE__ \
    )

#define __MACRO_SNPRINTF__(A,X,...) \
    do { \
        memset(A.str,'\0', __BUFFER_SIZE__); \
        if (!(A.sz = snprintf(A.str, __BUFFER_SIZE__, __VA_ARGS__))) { return X; } \
        if (A.sz > __BUFFER_SIZE__) { \
            __MACRO_SNPRINTF_OVERFLOW(A.sz, __BUFFER_SIZE__); \
        } \
    } while(0);

#define __MACRO_SNPRINTF_P__(A,X,...) \
    do { \
        memset(A->str,'\0', __BUFFER_SIZE__); \
        if (!(A->sz = snprintf(A->str, __BUFFER_SIZE__, __VA_ARGS__))) { return X; } \
        if (A->sz > __BUFFER_SIZE__) { \
            __MACRO_SNPRINTF_OVERFLOW(A->sz, __BUFFER_SIZE__); \
        } \
    } while(0);

#define __MACRO_SNPRINTF_SIZE__(A,X,S,...) \
    do { \
        memset(A.str,'\0', (S)); \
        if (!(A.sz = snprintf(A.str, (S), __VA_ARGS__))) { return X; } \
        if (A.sz > (S)) { \
            __MACRO_SNPRINTF_OVERFLOW(A.sz, (S)); \
        } \
    } while(0);

#define __BUFFER_CREATE__(A) \
    static char __EVALUTE_FUNC_CONCAT(A,__LINE__)[__BUFFER_SIZE__] = {0}; \
    string_s A = {0}; A.str = CP __EVALUTE_FUNC_CONCAT(A,__LINE__)

#define __BUFFER_CREATE_SIZE__(A,S) \
    static char __EVALUTE_FUNC_CONCAT(A,__LINE__)[S] = {0}; \
    string_s A = {0}; A.str = CP __EVALUTE_FUNC_CONCAT(A,__LINE__)

#define __BUFFER_POINTER__(A,P) \
    static string_s A = {0}; __SET_string_s(A,P)

#define __ADD_string_s(A) \
    .str = CP A, .sz = __STATIC_CONST_SIZE(A)

#define __SET_string_s(S,A) \
    S.str = A; S.sz = strlen(A)

#define __SET_string_size_s(S,A) \
    S.str = A; S.sz = __CSZ(A)

#define __BUILD_string_s(S,A) \
    static string_s S = {0}; S.str = A; S.sz = strlen(A)

#define __BUILD_string_static(S,A) \
    static string_s S = {0}; S.str = CP A; S.sz = (sizeof(A) - sizeof(char))

#define __BUILD_string_size(S,A,Z) \
    static string_s S = {0}; S.str = A; S.sz = Z

#define __MACRO_WAITPID__(A, B, S, T) \
    if (A != -1) { \
        if (waitpid((A), &(B), 0) == -1) { \
            __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not wait pid %d for children: %s", A, strerror(errno)); \
        } \
        __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_CHILD T " -> %s, exit code: %d, pid: %d", S, B, A); \
    }

typedef enum __core_in_file {
    IN_FILE_COMPLETE = 0,
    IN_FILE_PROCESS  = 1,
    IN_FILE_BTIMER   = 2,
    IN_FILE_BLIST    = 3,
    IN_FILE_PLIST    = 4,
    IN_FILE_BLXML    = 5,
    IN_FILE_BLZIP    = 6
} watch_file_e;

typedef struct __core_string_u_ {
    unsigned char *str;
    ssize_t        sz;
} string_u;

typedef struct __core_string_s_ {
    char *str;
    int   sz;
} string_s;

typedef struct __core_run_t_ {
    string_s  dir;
    int       ret;
    pid_t     pid;
} run_t;

typedef struct __core_watcher_t_ {
    int     cnt;
    int     pret;
    pid_t   pid;
    pid_t   dnspid;
    pid_t   urlpid;
    run_t **run;
} watcher_t;

typedef struct __core_portip_t_ {
    int       cnt;
    string_s  ip;
    string_s  port;
} fwall_pip_t;


typedef void *(*fwall_cb)(string_s*);

#include "./util/xmem.h"
#include "st-revizor-file.h"

int  watch_dir_checkin(string_s, watch_file_e);
void watch_dir_autoconfig(string_s*);
void watch_dir_setbusy(void);
void watch_set_autoconfig(void);
long proc_runing(char*);

FUNINLINE void __EV(auto_free,watcher_t)(watcher_t **x) {
    if ((x) && (*x)) {
        watcher_t *w = *x;
        if (w->run != NULL)
        {
            for (int i = 0; ((i < w->cnt) || (i == 0)); i++)
            {
                if (w->run[i] != NULL)
                {
                    free(w->run[i]);
                }
            }
            free(w->run);
        }
    }
}
FUNINLINE void __EV(auto_fclose,FILE)(FILE **x) {
    if ((x) && (*x)) { fclose(*(FILE**)x); }
}
FUNINLINE void __EV(auto_close,int)(int **x) {
    if ((x) && (*x)) { close(**(int**)x); }
}
FUNINLINE void __EV(auto_free,char)(char **x) {
    if ((x) && (*x)) { free(*(char**)x); }
}


#endif
