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

#if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
#include <syslog.h>
#endif

#define TFTP_FILE_COMPLETE "iplist.complete"
#define TFTP_FILE_PROCESS  "iplist.proccess"
#define TFTP_FILE_BLACKIP  "iplist.fw"
#define TFTP_FILE_URLLIST  "urllist.fw"
#define TFTP_FILE_WHITEIP  "whitelist.fw"
#define TFTP_FILE_PORT     "portlist.fw"
#define TFTP_FILE_PORTIP   "portiplist.fw"
#define TFTP_FILE_DNSLIST  "dnslist.fw"

#define EVALUTE_FUNC_CONCAT_(A,B) A ## B
#define EVALUTE_FUNC_CONCAT(A,B)  EVALUTE_FUNC_CONCAT_(A,B)

#define EVALUTE_TO_STR_(A) #A
#define EVALUTE_TO_STR(A) EVALUTE_TO_STR_(A)

#define __NELE(A)      (sizeof(A) / sizeof(A[0]))
#define __EV(A,B)      EVALUTE_FUNC_CONCAT(A,B)
#define __EVS(A)       EVALUTE_TO_STR(A)
#define __CSZ(A)       STATIC_CONST_SIZE(A)
#define __NOUSED       __attribute__((unused))
#define CP (char*)&

#if defined(__GNUC__) || defined(__GCC__)
#    define FUNALIAS(A) __attribute__((weak,alias(__EVS(A))))
#    define FUNINLINE static inline __attribute__((always_inline))
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

#define __MACRO_SNPRINTF__(A,X,...) \
    memset(A.str,'\0', __BUFFER_SIZE__); \
    if (!(A.sz = snprintf(A.str, __BUFFER_SIZE__, __VA_ARGS__))) { return X; }

#define __STATIC_CONST_SIZE(A) (int const) (sizeof(A)-1)

#define __BUFFER_SIZE__ 0xFF

#define __BUFFER_CREATE__(A) \
    char EVALUTE_FUNC_CONCAT(A,__LINE__)[__BUFFER_SIZE__] = {0}; \
    string_s A = {0}; A.str = CP EVALUTE_FUNC_CONCAT(A,__LINE__)

#define __BUFFER_POINTER__(A,P) \
    string_s A = {0}; __SET_string_s(A,P)

#define __ADD_string_s(A) \
    .str = CP A, .sz = __STATIC_CONST_SIZE(A)

#define __SET_string_s(S,A) \
    S.str = A; S.sz = strlen(A)

#define __BUILD_string_s(S,A) \
    string_s S = {0}; S.str = A; S.sz = strlen(A)

#if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
#   define __MACRO_TOSYSLOG__(A, ...) \
       syslog(A, __VA_ARGS__)
#else
#   if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
#      define __MACRO_TOSYSLOG__(A, ...)
#   else
#      define __MACRO_TOSYSLOG__(A, B, ...) \
          printf(B "\n", __VA_ARGS__)
#   endif
#endif

#if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
#   define __MACRO_TOSCREEN__(A, ...)
#else
#   define __MACRO_TOSCREEN__(A, ...) \
       printf(A "\n", __VA_ARGS__)
#endif

#define __MACRO_WAITPID__(A, B, S, T) \
    if (A != -1) { \
        if (waitpid((A), &(B), 0) == -1) { \
            __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not wait pid %d for children: %s", A, strerror(errno)); \
        } \
        __MACRO_TOSYSLOG__(LOG_INFO, "[Child Status]: " T " -> %s, exit code: %d, pid: %d", S, B, A); \
    }

enum __core_fwall_bin {
    ENUMBIN_IPT = 0,
    ENUMBIN_IPS = 1
};

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
    pid_t   httppid;
    run_t **run;
} watcher_t;

typedef struct __core_nfbin_t_ {
    char     *bin; /* wrapper bin */
    string_s  ipt;
    string_s  ips;
} nfbin_t;

typedef struct __core_portip_t_ {
    int       cnt;
    string_s  ip;
    string_s  port;
} fwall_pip_t;


typedef void *(*fwall_cb)(nfbin_t, string_s);

void clear_files(string_s);
int  file_exists(string_s, string_s);
void file_delete(string_s, string_s);
void file_backup(string_s, string_s);
void file_backup_fp(string_s, int);
void file_create(string_s, string_s, string_s);
void dir_parse(string_s);

//
// modules:
//
void main_module_parse(string_s, int);
void main_module_start(void);
void main_module_clear(void);
void main_module_end(void);
void main_module_dns(int);

#endif
