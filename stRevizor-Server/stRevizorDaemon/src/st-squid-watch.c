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

#define __NOT_HELP_INCLUDE 1
#define __WAIT_TIME (3 * 60)
#define __SFT_TIME "%x %I:%M"
#define __NEW_STAT "(new)"

#include "st-revizor-watch.h"
#include "st-revizor-inline.h"
#include "filter/st-revizor-lock.h"

static volatile int event_fd;
static volatile int event_sig;
static volatile void *event_data;

static void  proc_watcher(void);
static void  proc_exec(void);
static void  proc_change(time_t);
static void *proc_check(void*);

typedef struct __in_stat_watcher {
    string_s fname;
    struct stat stat;
    time_t ismodify;
} watch_stat;

watch_stat insource[] = {
    { {__ADD_string_s(HAVE_HTTP_PROXY_DIR "/" TFTP_FILE_SQUID_HTTP)}, {0}, 0 },
    { {__ADD_string_s(HAVE_HTTP_PROXY_DIR "/" TFTP_FILE_SQUID_HTTPS)}, {0}, 0 },
    { {__ADD_string_s(HAVE_HTTP_PROXY_DIR "/" TFTP_FILE_SQUID_PHTTP)}, {0}, 0 },
    { {__ADD_string_s(HAVE_HTTP_PROXY_DIR "/" TFTP_FILE_SQUID_PHTTPS)}, {0}, 0 }
};

int isrun = 1;
pthread_t t1;

static void handler_SIGRTMIN(int sig, siginfo_t *si, void *data)
{
    event_fd   = si->si_fd;
    event_sig  = sig;
    event_data = data;
}
static void main_SIGINT(__NOUSED int sig)
{
    __SYNC_MEM(isrun = 0);
}

int main(int argc, char *argv[])
{
    pid_t p;
    struct sigaction act;

    __BUILD_string_s(piddir,  CP HAVE_WATCH_PIDDIR);
    __BUILD_string_s(pidfile, CP SQPROCID ".pid");

    if (file_exists(&piddir, &pidfile))
    {
        if (proc_runing(CP SQPROCID) != -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ARUN, getpid(), piddir.str, pidfile.str);
            _exit(1);
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

        __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_START, SQPROCID, p);
        __BUFFER_CREATE__(pidcontext);
        __MACRO_SNPRINTF__(pidcontext, 2, "%d", p);
        file_create(&piddir, (string_s*)&pidfile, &pidcontext);

#   if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
        _exit(0);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not init daemon mode, %s", strerror(errno));
        _exit(2);
    }

    setsid();

    if (console_set() == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not change IO mode, pid: %d", getpid());
        _exit(2);
    }

#   endif

    act.sa_sigaction = handler_SIGRTMIN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN + 1, &act, NULL);

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  &main_SIGINT);

    if (pthread_create(&t1, NULL, proc_check, NULL) != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not create thread watcher, pid: %d", getpid());
        _exit(2);
    }

    proc_watcher();

    pthread_join(t1, NULL);

    file_delete(&piddir, &pidfile);
    __MACRO_TOSCREEN__(STR_WATCH_STOP, piddir.str, pidfile.str);

#   if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
    closelog();
#   endif
    _exit(0);
}
static void proc_watcher(void)
{
    __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Monitoring %s -> init", HAVE_HTTP_PROXY_DIR);

    int fd;

    if ((fd = open(HAVE_HTTP_PROXY_DIR, O_RDONLY)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not open directory %s (%s)", strerror(errno), HAVE_HTTP_PROXY_DIR);
        return;
    }
    if (fcntl(fd, F_SETSIG, SIGRTMIN + 1) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "not set signal SIGRTMIN %s (%s)", strerror(errno), HAVE_HTTP_PROXY_DIR);
        return;
    }

    proc_change(time(NULL));

    while (isrun)
    {
        time_t ctm = time(NULL);

        if (fcntl(fd, F_NOTIFY, DN_CREATE | DN_MODIFY) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "unsupport event DN_CREATE or DN_MODIFY %s (%s)", strerror(errno), HAVE_HTTP_PROXY_DIR);
            exit(2);
        }
        pause();
        proc_change(ctm);
    }
    close(fd);
}
static void proc_change(time_t ctm)
{
    struct stat fst;

    for (int i = 0; i < __NELE(insource); i++)
    {
        if (access(insource[i].fname.str, F_OK) == -1)
        {
            continue;
        }

        memset((void*)&fst, 0, sizeof(struct stat));

        if (stat(insource[i].fname.str, &fst) == 0)
        {
            if (insource[i].stat.st_mtime < fst.st_mtime)
            {
#               if defined(__MACRO_IS_TOSCREEN)
                struct tm *tmi;
                char tstr[2][100] = {{0},{0}};

                if (insource[i].stat.st_mtime > 0)
                {
                    tmi = localtime(&insource[i].stat.st_mtime);
                    strftime(tstr[0], 100, __SFT_TIME, tmi);
                }
                else
                {
                    memcpy(tstr[0], (void*) CP __NEW_STAT, __CSZ(__NEW_STAT));
                }

                tmi = localtime(&fst.st_mtime);
                strftime(tstr[1], 100, __SFT_TIME, tmi);

                __MACRO_TOSCREEN__(
                    STR_WATCH_DEBUG "-> file modify or create: %s, %s -> %s",
                    insource[i].fname.str,
                    tstr[0],
                    tstr[1]
                );
#               endif

                if (fst.st_mtime >= ctm)
                {
                    __SYNC_MEM(insource[i].ismodify = fst.st_mtime);
                }
                memcpy((void*)&insource[i].stat, (void*)&fst, sizeof(struct stat));
            }
        }
        else
        {
            __MACRO_TOSCREEN__(
                STR_WATCH_DEBUG "-> file statistic error: %s, %s",
                insource[i].fname.str,
                strerror(errno)
            );
        }
    }
}
static void *proc_check(__NOUSED void *arg)
{
    int i, chk;
    time_t ismodify, ctm;

#   if defined(__MACRO_IS_TOSCREEN)
    ctm = time(NULL);
    string_s tms = { ctime(&ctm), 0 };
    string_trim(&tms);

    __MACRO_TOSCREEN__(
        STR_WATCH_DEBUG "Start reconfigure Squid thread: %s",
        tms.str
    );
#   endif

    signal(SIGCHLD, SIG_IGN);

    while (isrun)
    {
        chk = 0;
        ctm = time(NULL);

        for (i = 0; i < __NELE(insource); i++)
        {
            __SYNC_MEM(ismodify = insource[i].ismodify);

            if ((ismodify > 0) && ((ismodify + __WAIT_TIME) < ctm))
            {
                chk = 1;
                __SYNC_MEM(insource[i].ismodify = 0);
            }
            else if (ismodify > 0)
            {
                chk = 0;
                break;
            }
        }
        if (chk == 1)
        {
#           if defined(__MACRO_IS_TOSCREEN)
            ctm = time(NULL);
            tms.str = ctime(&ctm);
            string_trim(&tms);

            __MACRO_TOSCREEN__(
                STR_WATCH_DEBUG "-> reconfigure Squid daemon: %s",
                tms.str
            );
#           endif
            proc_exec();
        }
        pthread_yield();
    }
    pthread_exit((void*)0);
}
static void proc_exec(void)
{
    pid_t p = fork();
    if (!p)
    {
        char *data[] = {
            CP "/bin/bash",
            CP "-c",
            CP HAVE_HTTP_PROXY_BIN " -k reconfigure ; exit ;",
            NULL
        };

#       if (defined(HAVE_EXECVPE) && (HAVE_EXECVPE == 1))
        if (execvpe(data[0], data, NULL) == -1)
#       else
        if (execve(data[0], data, NULL) == -1)
#       endif
        {
            __MACRO_TOSYSLOG__(LOG_ERR,
                STR_WATCH_DEBUG "-> exec reconfigure Squid error: %s (%s)",
                strerror(errno),
                data[0]
            );
        }
        _exit(127);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR,
            STR_WATCH_DEBUG "-> fork proc reconfigure Squid error: %s",
            strerror(errno)
        );
    }
}
