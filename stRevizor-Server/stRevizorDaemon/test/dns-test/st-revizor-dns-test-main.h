
#ifndef REVIZOR_DNS_TEST_H
#define REVIZOR_DNS_TEST_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <errno.h>
#include <pthread.h>

#if defined(__linux) || defined(__linux__)
#   if  !defined(PR_SET_NAME)
#        define PR_SET_NAME 15
#   endif
#   if  !defined(PR_GET_NAME)
#        define PR_GET_NAME 16
#   endif
#   include <sys/prctl.h>
#endif

#if !defined(HAVE_CONFIG_H_LOAD)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../../config.h"
#endif

#define REVIZOR_DNS_SERVER "127.0.0.1"
#define OUTDIR_DNS_REPORT  "dns-test-result"
#define TFTPDIR_DNS_REPORT HAVE_WATCH_PATH "/dns-test.stat"

#include "../../src/util/xbuild.h"
#include "../../src/util/xmem.h"
#include "../../src/st-revizor-lang.h"
#include "../../src/st-revizor-watch.h"
#include "../../src/filter/st-revizor-net.h"
#include "../../src/filter/st-revizor-lock.h"
#include "../../src/util/thpool.h"

#define __MACRO_TOSCREEN__(A, ...) printf(A "\n", __VA_ARGS__)
#define __MACRO_IS_TOSCREEN 1

#include "../../src/st-revizor-inline.h"
#include "st-revizor-dns-test-request.h"

#define STR_PRN_CREATE "Create %s directory: %s"
#define STR_PRN_REPORT "\n\tTotal check DNS entry: %lu, NOT blocked: %lu, hosts errors: %lu\n\tScan CPU times: %.2f sec.\n\n"
#define STR_ERR_NOTF   "Error: %s file not found: '%s' - %s"
#define STR_ERR_OPEN   "Error: unable open %s file: '%s' - %s"
#define STR_ERR_CREATE "Error: unable create %s: '%s' - %s"
#define STR_ERR_ALLOC  "Error: memory allocate, '%s' - %s"
#define STR_PRN_ACCEPT "-* [ACCEPT]:\t%s\n"
#define STR_PRN_BLOCK  "-> [BLOCK]: \t%s\n"
#define STR_PRN_ERROR  "-! [ERROR]: \t%s\n"
#define STR_FWR_ACCEPT "%lu\t-* [ACCEPT]:\t%s%s\n"
#define STR_FWR_ERROR  "%lu\t-! [ERROR]: \t%s\t\t%s\n"

#define STR_PRN_REPORT_0 "\tTest DNS result %s"
#define STR_PRN_REPORT_1 "\tOriginal date reester: %s"
#define STR_PRN_REPORT_2 "\tReester date unknown"
#define STR_PRN_REPORT_3 "\n\t'ACCEPT' is non-blocking DNS address"
#define STR_PRN_REPORT_4 "\n\t'ERROR' is communication error"
#define STR_PRN_REPORT_5 "\n\t'BLOCK' is blocking DNS address\n\n"

typedef struct __test_th_data_t_ {
    FILE         *fi, *fo;
    unsigned long cnt, bcnt, ecnt, *lbcnt, *lecnt;
    clock_t       start, stop;
    string_s      data;
} test_th_data_t;


#endif
