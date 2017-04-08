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
#include "version/st-revizor-build-info.h"

static volatile int event_fd;
static volatile int event_sig;
static volatile void *event_data;

static int  _isblocktime = 0;

static int  proc_watcher(string_s, int);

#if (defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1))
static int  proc_dnsfilter(void);
int dns_filter(void);
#endif

#if (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1))
static int  proc_urlfilter(void);
int url_filter(void);
#endif

#if (defined(HAVE_XML_PARSER_ENABLE) && (HAVE_XML_PARSER_ENABLE == 1))
int plist_zip_prog(string_s*, const string_s*, const string_s*, const string_s*);
int plist_xml_prog(string_s*, const string_s*, const string_s*);
#endif

static int  watch_checktimeblock(void);
static void watch_timeblock(string_s);

const string_s insource[] = {
    { __ADD_string_s(TFTP_FILE_COMPLETE) },
    { __ADD_string_s(TFTP_FILE_PROCESS)  },
    { __ADD_string_s(TFTP_FILE_BTIMER)   },
    { __ADD_string_s(TFTP_FILE_BLIST)    },
    { __ADD_string_s(TFTP_FILE_PLIST)    },
    { __ADD_string_s(TFTP_FILE_BLXML)    },
    { __ADD_string_s(TFTP_FILE_BLZIP)    }
};
int inprocess[__NELE(insource)];
int isrun = 1;

watcher_t wrun = {0};

static void handler_SIGRTMIN(int sig, siginfo_t *si, void *data)
{
    event_fd   = si->si_fd;
    event_sig  = sig;
    event_data = data;
}
static void parent_SIG(int sig)
{
    if (sig != SIGUSR2)
    {
        for (int i = 0; ((i < wrun.cnt) || (i == 0)); i++)
        {
            if (wrun.run[i]->pid != -1)
            {
                kill(wrun.run[i]->pid, sig);
            }
        }
    }
    if (sig != SIGUSR1)
    {
        if (wrun.dnspid != -1)
        {
            kill(wrun.dnspid, sig);
        }
        if (wrun.urlpid != -1)
        {
            kill(wrun.urlpid, sig);
        }
    }
}
static void child_SIGINT(__NOUSED int sig)
{
    __SYNC_MEM(isrun = 0);
}
static void child_SIGUSR1(__NOUSED int sig)
{
    watch_dir_setbusy();
}
void watch_set_autoconfig(void)
{
    int i = 0;
    for (i = 0; ((i < wrun.cnt) || (i == 0)); i++)
    {
        __MACRO_TOSCREEN__(STR_WATCH_DEBUG "Create auto config -> %s/%s.(json|xml)", wrun.run[i]->dir.str, CP TFTP_FILE_AUTOCONF);
        watch_dir_autoconfig(&wrun.run[i]->dir);
    }
}

int main(int argc, char *argv[])
{
#   include "include/st-revizor-watch-helper.c"

/*
    __BUILD_string_s(fpath,  CP "./");
    plist_xml_prog(&fpath, &insource[IN_FILE_BLXML], &insource[IN_FILE_BLIST]);
    exit(0);
*/

    int  i;
    pid_t p;
    struct sigaction act;
    wrun.cnt = ((argc > 1) ? (argc - 1) : 0);
    wrun.run = (run_t**)calloc((size_t)(wrun.cnt + 1), (size_t)sizeof(run_t*));
    wrun.dnspid = wrun.urlpid = -1;
    memset(inprocess, 0, (__NELE(insource) * sizeof(int)));

    __cleanup_set_free(watcher_t, wrunfree, &wrun);

    __BUILD_string_s(piddir,  CP HAVE_WATCH_PIDDIR);
    __BUILD_string_s(pidfile, CP PROCID ".pid");

#   if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
    // setlogmask(LOG_UPTO(LOG_NOTICE));
    // openlog(PROCID, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
#   endif

    if (file_exists(&piddir, &pidfile))
    {
        if (proc_runing(CP PROCID) != -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ARUN, getpid(), piddir.str, pidfile.str);
            __cleanup_notsupport_p(auto_freewatcher_t, wrunfree);
            _exit(1);
        }
    }

#   include "include/st-revizor-chk-libdir.c"

    if (wrun.cnt == 0)
    {
        wrun.run[0] = (run_t*)calloc(1, (size_t)sizeof(run_t));
        __SET_string_s(wrun.run[0]->dir, HAVE_WATCH_PATH);
    }
    else if (wrun.cnt > 0)
    {
        for (i = 0; i < wrun.cnt; i++)
        {
            wrun.run[i] = (run_t*)calloc(1, (size_t)sizeof(run_t));
            __SET_string_s(wrun.run[i]->dir, argv[(i + 1)]);
        }
    }

#   if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
    fflush(0);
    p = fork();
    if (p > 0)
    {
#   else
        p = getpid();
#   endif

        __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_START, PROCID, p);
        __BUFFER_CREATE__(pidcontext);
        __MACRO_SNPRINTF__(pidcontext, 2, "%d", p);
        file_create(&piddir, (string_s*)&pidfile, &pidcontext);

#   if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
        __cleanup_notsupport_p(auto_freewatcher_t, wrunfree);
        _exit(0);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not init daemon mode, %s", strerror(errno));
        __cleanup_notsupport_p(auto_freewatcher_t, wrunfree);
        _exit(2);
    }

    setsid();

    if (console_set() == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not change IO mode, pid: %d", getpid());
        __cleanup_notsupport_p(auto_freewatcher_t, wrunfree);
        _exit(2);
    }

#   endif

    act.sa_sigaction = handler_SIGRTMIN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN + 1, &act, NULL);
    wrun.pid = p;

    // create watchers child
#   include "include/st-revizor-proc-watch-fork.c"
    // create dns filter child
#   include "include/st-revizor-proc-dnsfilter-fork.c"
    // create url filter child
#   include "include/st-revizor-proc-urlfilter-fork.c"

    signal(SIGUSR1, &parent_SIG);
    signal(SIGUSR2, &parent_SIG);
    signal(SIGINT,  &parent_SIG);

    // restore ipfw, clear input directory
#   include "include/st-revizor-proc-watch-pack.c"
    // wait watchers child
#   include "include/st-revizor-proc-watch-wait.c"
    // wait dns filter child
#   include "include/st-revizor-proc-dnsfilter-wait.c"
    // wait dns filter child
#   include "include/st-revizor-proc-urlfilter-wait.c"

    file_delete(&piddir, &pidfile);
    __MACRO_TOSCREEN__(STR_WATCH_STOP, piddir.str, pidfile.str);

#   if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
    closelog();
#   endif
    __cleanup_notsupport_p(auto_freewatcher_t, wrunfree);
    _exit(0);
}

#if (defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1))
static int proc_dnsfilter(void)
{
    __MACRO_TOSYSLOG__(LOG_INFO, STR_DNS_DEBUG "Network DNS filter -> %s", "init");

    pid_t p = fork();
    if (p > 0)
    {
        return p;
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Not init DNS filter: %s", strerror(errno));
        return p;
    }

    __BUILD_string_size(procname, CP "dnsfilter", -1);
    proc_name_set((string_s*)&procname);

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  &child_SIGINT);

    if (dns_filter() == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Return from run DNS filter: %s", strerror(errno));
    }
    _exit(p);
}
#endif

#if (defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1))
static int proc_urlfilter(void)
{
    __MACRO_TOSYSLOG__(LOG_INFO, STR_URL_DEBUG "Network URL filter -> %s", "init");

    pid_t p = fork();
    if (p > 0)
    {
        return p;
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Not init URL filter: %s", strerror(errno));
        return p;
    }

    __BUILD_string_size(procname, CP "urlfilter", -1);
    proc_name_set((string_s*)&procname);

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  &child_SIGINT);

    if (url_filter() == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Return from run URL filter: %s", strerror(errno));
    }
    _exit(p);
}
#endif
static int proc_watcher(string_s watchpath, int idx)
{
    __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Monitoring %s -> init: index %d", watchpath.str, idx);

    int fd;

    if ((fd = open(watchpath.str, O_RDONLY)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not open directory %s (%s)", strerror(errno), watchpath.str);
        return -1;
    }
    if (fcntl(fd, F_SETSIG, SIGRTMIN + 1) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not set signal SIGRTMIN %s (%s)", strerror(errno), watchpath.str);
        return -1;
    }

    pid_t p = fork();
    if (p > 0)
    {
        return p;
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not init watch: %s (%s)", strerror(errno), watchpath.str);
        return p;
    }

    __BUILD_string_size(procname, CP "watcher", idx);
    proc_name_set((string_s*)&procname);

    signal(SIGUSR1, &child_SIGUSR1);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  &child_SIGINT);

    __BUFFER_CREATE__(fpath);

    __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Monitoring %s -> start: pid %d", watchpath.str, getpid());
    file_clears(&watchpath, (string_s*)insource, __NELE(insource));
    watch_set_autoconfig();

    while (isrun)
    {
        if (fcntl(fd, F_NOTIFY, DN_CREATE) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "unsupport event DN_CREATE %s (%s)", strerror(errno), watchpath.str);
            exit(2);
        }
        pause();

#       if (defined(HAVE_XML_PARSER_ENABLE) && (HAVE_XML_PARSER_ENABLE == 1))
__label_set(drescan);
#       endif

        if (watch_checktimeblock())
        {
            file_clears(&watchpath, insource, __NELE(insource));
            continue;
        }
        else if (watch_dir_checkin(watchpath, IN_FILE_BTIMER))
        {
            __MACRO_SNPRINTF__(fpath, p, "%s/%s", watchpath.str, insource[IN_FILE_BTIMER].str);
            watch_timeblock(fpath);
            file_delete_fp(&fpath);
        }
        else if (watch_dir_checkin(watchpath, IN_FILE_BLIST))
        {
            time_t t = time(NULL);
            struct tm *ltm = localtime(&t);

            kill(wrun.pid, SIGUSR1);

            __BUFFER_CREATE__(fcontext);
            __MACRO_SNPRINTF__(fcontext, -1, "%04d/%02d/%02d  %02d:%02d:%02d",
                (ltm->tm_year+1900), (ltm->tm_mon+1), ltm->tm_mday,
                ltm->tm_hour, ltm->tm_min, ltm->tm_sec
            );
            file_create(&watchpath, (string_s*)&insource[IN_FILE_PROCESS], &fcontext);
            __MACRO_TOSYSLOG__(LOG_INFO, STR_IPT_UPDATE "-> Monitoring id: %d", fcontext.str, idx);
            __MACRO_SNPRINTF__(fpath, p, "%s/%s", watchpath.str, insource[IN_FILE_BLIST].str);

            ipfw_exec(IPFW_LSTSTART, NULL);
            ipfw_parse(fpath);
            ipfw_exec(IPFW_LSTEND, NULL);

#           if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
            ipfw_exec(IPFW_SQUID, NULL);
#           endif

            file_backup(&watchpath, (string_s*)&insource[IN_FILE_BLIST]);
            file_clears(&watchpath, (string_s*)insource, __NELE(insource));
            kill(wrun.pid, SIGUSR2);
            kill(wrun.pid, SIGUSR1);
        }
#       if (defined(HAVE_XML_PARSER_ENABLE) && (HAVE_XML_PARSER_ENABLE == 1))

        else if (watch_dir_checkin(watchpath, IN_FILE_BLXML))
        {
            __MACRO_TOSCREEN__(
                STR_WATCH_DEBUG "-> Begin XML parse: %s/%s",
                watchpath.str,
                insource[IN_FILE_BLXML].str
            );
            if (plist_xml_prog(&watchpath, &insource[IN_FILE_BLXML], &insource[IN_FILE_BLIST]) == -1)
            {
                __MACRO_TOSYSLOG__(LOG_ERR,
                    STR_WATCH_ERROR "-> Error XML parse: %s/%s",
                    watchpath.str,
                    insource[IN_FILE_BLXML].str
                );
                file_delete(&watchpath, (string_s*)&insource[IN_FILE_BLXML]);
                file_delete(&watchpath, (string_s*)&insource[IN_FILE_BLIST]);
                file_clears(&watchpath, (string_s*)insource, __NELE(insource));
            }
            else
            {
                __MACRO_TOSCREEN__(
                    STR_WATCH_DEBUG "-> End XML parse: %s/%s",
                    watchpath.str,
                    insource[IN_FILE_BLXML].str
                );
                if (file_exists(&watchpath, (string_s*)&insource[IN_FILE_BLZIP]))
                {
                    file_backup(&watchpath, (string_s*)&insource[IN_FILE_BLZIP]);
                    file_delete(&watchpath, (string_s*)&insource[IN_FILE_BLXML]);
                }
                else
                if (file_exists(&watchpath, (string_s*)&insource[IN_FILE_BLXML]))
                {
                    file_backup(&watchpath, (string_s*)&insource[IN_FILE_BLXML]);
                }
                watch_dir_autoconfig(&watchpath);
                goto drescan;
            }
        }
        else if (watch_dir_checkin(watchpath, IN_FILE_BLZIP))
        {
            __MACRO_TOSCREEN__(
                STR_WATCH_DEBUG "-> Begin unzip archive: %s/%s",
                watchpath.str,
                insource[IN_FILE_BLZIP].str
            );
            if (plist_zip_prog(&watchpath, &insource[IN_FILE_BLZIP], &insource[IN_FILE_BLXML], &insource[IN_FILE_BLIST]) == -1)
            {
                __MACRO_TOSYSLOG__(LOG_ERR,
                    STR_WATCH_ERROR "-> Error unzip archive: %s/%s",
                    watchpath.str,
                    insource[IN_FILE_BLZIP].str
                );
                file_delete(&watchpath, (string_s*)&insource[IN_FILE_BLZIP]);
                file_delete(&watchpath, (string_s*)&insource[IN_FILE_BLXML]);
                file_clears(&watchpath, (string_s*)insource, __NELE(insource));
            }
            else
            {
                __MACRO_TOSCREEN__(
                    STR_WATCH_DEBUG "-> Unzip archive done: %s/%s",
                    watchpath.str,
                    insource[IN_FILE_BLZIP].str
                );
                goto drescan;
            }
        }
#       endif
    }
    close(fd);
    _exit(p);
}

/* Filters module IPFW timer disable access menegement */

static int watch_checktimeblock(void)
{
    return ((_isblocktime) ? 1 : 0);
}

static void * watch_timeblock_pth_(void *arg)
{
    if ((arg == NULL) || (*((int*)(arg)) <= 0))
    {
        pthread_exit((void*)-1);
    }

    int cnt = 0, csec = 0, bsec = (*((int*)(arg)) * 60);
#   if defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)
    __BUFFER_CREATE__(ppath);
    __MACRO_SNPRINTF__(ppath, NULL, "%s/%s", HAVE_WATCH_BACKUP, insource[IN_FILE_PLIST].str);
    char *param[2] = { ppath.str, NULL };
#   endif

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_testcancel();

    if (memcmp(CP HAVE_DNS_FILTRED, CP HAVE_HTTP_FILTRED, sizeof(HAVE_DNS_FILTRED)) == 0)
    {
        __MACRO_TOSYSLOG__(LOG_INFO,
            STR_IPFW_INFO "Block " HAVE_DNS_FILTRED " all traffic -> %d sec",
            bsec
        );
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_INFO,
            STR_IPFW_INFO "Block " HAVE_DNS_FILTRED "-" HAVE_HTTP_FILTRED " all traffic -> %d sec",
            bsec
        );
    }
    ipfw_exec(IPFW_CTRLSTART, NULL);
#   if defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)
    ipfw_exec(IPFW_DNSDEL,NULL);
#   endif
#   if defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)
    ipfw_exec(IPFW_URLDEL, (char**)param);
#   endif

    clock_t diff0, diff1;
    if ((diff0 = clock()) < 0)
    {
        pthread_exit((void*)-1);
    }
    do {
        if ((diff1 = (clock() - diff0)) < 0) { break; }
        csec = (diff1 / CLOCKS_PER_SEC);
        if (((csec % 60) == 0) && (cnt != csec))
        {
            __MACRO_TOSCREEN__(STR_IPFW_INFO "Wait to end blocking traffic, left: %d sec", (bsec - csec));
            cnt = csec;
        }
        pthread_yield();

    } while (csec < bsec);

#   if defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)
    ipfw_exec(IPFW_DNSADD,NULL);
#   endif
#   if defined(HAVE_HTTP_FILTER_ENABLE) && (HAVE_HTTP_FILTER_ENABLE == 1)
    ipfw_exec(IPFW_URLADD, (char**)param);
#   endif
    ipfw_exec(IPFW_CTRLEND,NULL);


    // TODO: rename backup table le /16 to REVIZOR chain

    __SYNC_MEM(*((int*)(arg)) = 0);

    __MACRO_TOSYSLOG__(LOG_INFO,
        STR_IPFW_INFO "Block period -> %d sec END",
        bsec
    );

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_exit((void*)0);
}
static void watch_timeblock(string_s fpath)
{
    __BUFFER_CREATE__(btime);
    __SYNC_MEM(
        if (_isblocktime)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Another block-time period in use.. %s", "Skip.");
            return;
        }
    );
    if (!file_exists_fp(&fpath))
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Not found block-time file -> %s", fpath.str);
        return;
    }
    __cleanup_new_fclose(FILE) fp = NULL;

    do {
        if ((fp = fopen(fpath.str, "r")) == NULL) { break; }
        if ((btime.sz = fread(btime.str, sizeof(char), __BUFFER_SIZE__, fp)) <= 0) { break; }
        if (btime.str == NULL) { break; }
        __SYNC_MEM(
            if (_isblocktime) { break; }
            if ((_isblocktime = atoi(btime.str)) == 0) { break; }
        );
        __cleanup_notsupport(fclose, fp);
        do {
            pthread_t t1;
            pthread_attr_t thattr;

            if (pthread_attr_init(&thattr) != 0) { break; }
            if (pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_DETACHED) != 0) { break; }
            if (pthread_create(&t1, &thattr, watch_timeblock_pth_, (void*)&_isblocktime) != 0) { break; }
            if (pthread_attr_destroy(&thattr) != 0) { break; }
            return;

        } while(0);

        __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Error: block-time pthread -> %s", strerror(errno));
        __SYNC_MEM(_isblocktime = 0);
        return;

    } while(0);

    if (fp != NULL)
    {
        __cleanup_notsupport(fclose, fp);
    }
    __MACRO_TOSYSLOG__(LOG_ERR, STR_IPFW_ERROR "Error: block-time file %s -> %s", fpath.str, strerror(errno));
    __SYNC_MEM(_isblocktime = 0);
}
