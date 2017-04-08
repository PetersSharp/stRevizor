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

#define PROCID "revizor-watch"

static volatile int event_fd;
static volatile int event_sig;
static volatile void *event_data;

static int proc_dnsfilter(void);
static int proc_watcher(string_s);

const string_s insource[] = {
    { __ADD_string_s(TFTP_FILE_COMPLETE) },
    { __ADD_string_s(TFTP_FILE_PROCESS)  },
    { __ADD_string_s(TFTP_FILE_BLACKIP)  },
    { __ADD_string_s(TFTP_FILE_URLLIST)  },
    { __ADD_string_s(TFTP_FILE_WHITEIP)  },
    { __ADD_string_s(TFTP_FILE_PORT)     },
    { __ADD_string_s(TFTP_FILE_PORTIP)   },
    { __ADD_string_s(TFTP_FILE_DNSLIST)  }
};
int inprocess[__NELE(insource)];
int isrun = 1, isbusy = 0;

watcher_t wrun = {0};
nfbin_t binpath = {0};

static void handler_SIGRTMIN(int sig, siginfo_t *si, void *data)
{
    event_fd = si->si_fd;
    event_sig = sig;
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
    if ((sig != SIGUSR1) && (wrun.dnspid != -1))
    {
        kill(wrun.dnspid, sig);
    }
}
static void child_SIGINT(__NOUSED int sig)
{
    isrun = 0;
}
static void child_SIGUSR1(__NOUSED int sig)
{
    isbusy = ((isbusy) ? 0 : 1);
}
#if defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)
int dns_filter(void);
int dns_init_table(void);

static void child_SIGUSR2(__NOUSED int sig)
{
#if defined(HAVE_DNS_CLEAR_RULE) && (HAVE_DNS_CLEAR_RULE == 1)
    main_module_dns(0);
#endif

    if (dns_init_table() != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not update hash table from signal, %s", strerror(errno));
    }
}
#endif

int main(int argc, char *argv[])
{
    int  i;
    pid_t p;
    struct sigaction act;
    wrun.cnt = ((argc > 1) ? (argc - 1) : 0);
    wrun.run = (run_t**)calloc((size_t)(wrun.cnt + 1), (size_t)sizeof(run_t*));
    memset(inprocess, 0, (__NELE(insource) * sizeof(int)));

    __BUILD_string_s(piddir, HAVE_WATCH_PIDDIR);
    __BUILD_string_s(pidfile, PROCID ".pid");

#   if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(PROCID, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
#   endif

#   include "include/st-revizor-chk-libdir.c"

#   if defined(HAVE_IPT_ENABLE) && (HAVE_IPT_ENABLE == 1)
#     include "include/st-revizor-chk-ipt-bin.c"
#   else
#     error "(Netfilter) iptables not installed or support for you system"
#   endif

#   if defined(HAVE_IPS_ENABLE) && (HAVE_IPS_ENABLE == 1)
#     include "include/st-revizor-chk-ips-bin.c"
#   else
#     warning "(Netfilter) ipset not installed or support for you system"
#   endif

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
    p = fork();
    if (p > 0)
    {
#   else
        p = getpid();
#   endif

        __MACRO_TOSYSLOG__(LOG_INFO, "[Start]: success, daemon pid %d", p);
        __BUFFER_CREATE__(pidcontext);
        __MACRO_SNPRINTF__(pidcontext, 2, "%d", p);
        file_create(piddir, pidfile, pidcontext);

#   if defined(HAVE_WATCH_DAEMON) && (HAVE_WATCH_DAEMON == 1)
        _exit(0);
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not init daemon mode, %s", strerror(errno));
        _exit(2);
    }

    int io = -1;

    setsid();

    for (i = getdtablesize(); i >= 0; --i)
    {
        close(i);
    }
    if ((io = open("/dev/null", O_RDWR)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not open /dev/null, %s", strerror(errno));
        _exit(2);
    }
    else
    {
        if (fcntl(io, F_SETFD, FD_CLOEXEC) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not set set autoclose, %s", strerror(errno));
            _exit(2);
        }
        dup(io); dup(io);
        setvbuf(fdopen(io,"r"), NULL, _IONBF, 0);
    }
#   endif

    act.sa_sigaction = handler_SIGRTMIN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN + 1, &act, NULL);
    wrun.pid = p;

    // watchers child
#   include "include/st-revizor-proc-watch-fork.c"
    // dns filter child
#   include "include/st-revizor-proc-dnsfilter-fork.c"

    signal(SIGUSR1, &parent_SIG);
    signal(SIGUSR2, &parent_SIG);
    signal(SIGINT,  &parent_SIG);

    // watchers child
#   include "include/st-revizor-proc-watch-wait.c"
    // dns filter child
#   include "include/st-revizor-proc-dnsfilter-wait.c"


    main_module_dns(0);

    file_delete(piddir, pidfile);
#   if defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)
    closelog();
#   endif
}

#if defined(HAVE_DNS_FILTER_ENABLE) && (HAVE_DNS_FILTER_ENABLE == 1)
static int proc_dnsfilter()
{
    pid_t p = fork();
    if (p > 0)
    {
        return p;
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not init DNS filter: %s", strerror(errno));
        return p;
    }

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, &child_SIGUSR2);
    signal(SIGINT,  &child_SIGINT);

    if (dns_filter() == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Status]: return run DNS filter: %s", strerror(errno));
    }
    _exit(p);
}
#endif
static int proc_watcher(string_s watchpath)
{
    int fd;

    if ((fd = open(watchpath.str, O_RDONLY)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not open directory %s (%s)", strerror(errno), watchpath.str);
        return -1;
    }
    if (fcntl(fd, F_SETSIG, SIGRTMIN + 1) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not set signal SIGRTMIN %s (%s)", strerror(errno), watchpath.str);
        return -1;
    }

    pid_t p = fork();
    if (p > 0)
    {
        return p;
    }
    else if (p == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not init watch: %s (%s)", strerror(errno), watchpath.str);
        return p;
    }

    signal(SIGUSR1, &child_SIGUSR1);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  &child_SIGINT);

    while (isrun)
    {
        if (fcntl(fd, F_NOTIFY, DN_CREATE) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: unsupport event DN_CREATE %s (%s)", strerror(errno), watchpath.str);
            exit(2);
        }
        pause();

        if (
            (!isbusy) &&
            (file_exists(watchpath, insource[0]) == 1) &&
            (file_exists(watchpath, insource[1]) == 0)
           )
        {
            kill(wrun.pid, SIGUSR1);

            time_t t = time(NULL);
            struct tm *ltm = localtime(&t);

            __BUFFER_CREATE__(fcontext);
            __MACRO_SNPRINTF__(fcontext, -1, "%04d/%02d/%02d  %02d:%02d:%02d",
                (ltm->tm_year+1900), (ltm->tm_mon+1), ltm->tm_mday,
                ltm->tm_hour, ltm->tm_min, ltm->tm_sec
            );
            file_create(watchpath, insource[1], fcontext);
            __MACRO_TOSYSLOG__(LOG_INFO, "[Update fwall]: %s", fcontext.str);

            dir_parse(watchpath);
            clear_files(watchpath);

            kill(wrun.pid, SIGUSR1);
        }
    }
    close(fd);
    _exit(p);
}
void clear_files(string_s fdir)
{
    DIR *directory;
    struct dirent* file;

    if ((directory = opendir(fdir.str)) != NULL) {
        while ((file = readdir(directory)) != NULL) {
            if (file->d_type == DT_REG)
            {
                for (int i = 0; i < __NELE(insource); i++)
                {
                    if (memcmp(insource[i].str, file->d_name, insource[i].sz) == 0)
                    {
                        switch(i)
                        {
                            case 0:
                            case 1:
                            {
                                file_delete(fdir, insource[i]);
                                break;
                            }
                            default:
                            {
                                file_backup(fdir, insource[i]);
                                break;
                            }
                        }
                    }
                }
            }
        }
        closedir(directory);
    }
}
int file_exists(string_s fdir, string_s fname)
{
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, 0, "%s/%s", fdir.str, fname.str);
    if (access(fpath.str, F_OK) != -1)
    {
        return 1;
    }
    return 0;
}
void file_delete(string_s fdir, string_s fname)
{
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir.str, fname.str);
    if (remove(fpath.str) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: unable to delete %s, %s", fpath.str, strerror(errno));
        errno = 0;
    }
}
void file_create(string_s fdir, string_s fname, string_s content)
{
    int fd;
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir.str, fname.str);

    if ((fd = open(fpath.str, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: not open pid file:  %s (%s)", strerror(errno), fpath.str);
        exit(2);
    }
    write(fd, content.str, content.sz);
    close(fd);
}
static void file_backup_(string_s fpath, string_s opath)
{
    struct stat st = {0};
    if (stat(HAVE_WATCH_BACKUP, &st) == -1)
    {
        if (mkdir(HAVE_WATCH_BACKUP, 0775) == -1)
        {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: unable create backup directory %s, %s", HAVE_WATCH_BACKUP, strerror(errno));
        errno = 0;
        return;
        }
    }
    if (rename(fpath.str, opath.str) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: unable to move %s, %s", fpath.str, strerror(errno));
        errno = 0;
    }
}
void file_backup(string_s fdir, string_s fname)
{
    __BUFFER_CREATE__(fpath);
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir.str, fname.str);
    __MACRO_SNPRINTF__(opath, , "%s/%s", HAVE_WATCH_BACKUP, fname.str);
    file_backup_(fpath, opath);
}
void file_backup_fp(string_s fpath, int n)
{
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(opath, , "%s/%s", HAVE_WATCH_BACKUP, insource[n].str);
    file_backup_(fpath, opath);
}
void dir_parse(string_s fdir)
{
    int isresult = 0;
    __BUFFER_CREATE__(fpath);

    main_module_start();

    for (int i = 2; i < __NELE(insource); i++)
    {
        __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir.str, insource[i].str);
        if (access(fpath.str, F_OK) == -1)
        {
            continue;
        }
        if (inprocess[i] == 1)
        {
            continue;
        }

        __MACRO_TOSYSLOG__(LOG_INFO, "[Found source]: %s", insource[i].str);

        isresult++;
        inprocess[i] = 1;
        main_module_parse(fpath, i);
    }
    for (int i = 2; i < __NELE(insource); i++)
    {
        inprocess[i] = 0;
    }
    if (isresult > 0)
    {
        main_module_end();
    }
    else
    {
        main_module_clear();
    }
}
