
#include "st-revizor-dns-test-main.h"

static int isrun                        = 1;
static int isnoprn                      = 0;
static st_table_lock_t wrlock           = {0};
static char reportname[__BUFFER_SIZE__] = {0};

static void sig_SIGINT(__NOUSED int sig)
{
    __SYNC_MEM(isrun = 0);
}

static void set_noprint(int argc, char **argv)
{
    if (argc > 1)
    {
        if ((strlen(argv[1]) == 2) && (argv[1][0] == '-') && (argv[1][1] == 'q'))
        {
            __SYNC_MEM(isnoprn = 1);
        }
    }
    if (!isnoprn)
    {
        __MACRO_TOSCREEN__(
            "\n\tRun %s -q (quiet any message)" \
            "\n\tstRevizor v.%s test DNS part (https://github.com/PetersSharp/stRevizor)\n\n",
            argv[0], HAVE_BUILD_VER
        );
    }
}

static void set_report_symlink(void)
{
    struct stat buf;
    if (lstat(CP TFTPDIR_DNS_REPORT, &buf) == 0)
    {
        unlink(CP TFTPDIR_DNS_REPORT);
    }
    if (symlink((const char*)&reportname, (const char*)& TFTPDIR_DNS_REPORT) != 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_CREATE, CP "report symlink", CP TFTPDIR_DNS_REPORT, strerror(errno));
    }
}

static void write_io_notblock(test_th_data_t *td, char *fmt, int err)
{
    __TABLE_WLOCK(&wrlock,
        fprintf(td->fo,
            fmt, td->cnt, td->data.str,
            ((!err) ? CP "" : strerror(err))
        );
    );
}

static void * th_run(void *arg)
{
    int err = 0;
    test_th_data_t *td = (test_th_data_t*)arg;
    switch (dns_gethostbyname(&td->data, T_A, &err))
    {
        case -1:
        {
            __SYNC_MEM(++(*td->lecnt));
            write_io_notblock(td, CP STR_FWR_ERROR, err);
            if (!isnoprn) { printf(STR_PRN_ERROR, td->data.str); }
            break;
        }
        case 0:
        {
            if (!isnoprn) { printf(STR_PRN_BLOCK, td->data.str); }
            break;
        }
        default:
        {
            __SYNC_MEM(++(*td->lbcnt));
            write_io_notblock(td, CP STR_FWR_ACCEPT, 0);
            if (!isnoprn) { printf(STR_PRN_ACCEPT, td->data.str); }
            break;
        }
    }
    free(td->data.str);
    free(td);
    return NULL;
}

static int init_io(test_th_data_t *td)
{
    struct stat st = {0};
    __BUFFER_CREATE__(bpath);

    do
    {
        if (stat(HAVE_WATCH_BACKUP, &st) == -1)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK "Create backup directory %s\n", HAVE_WATCH_BACKUP);

            if (mkdir(HAVE_WATCH_BACKUP, 0775) == -1)
            {
                __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: unable create backup directory %s, %s", HAVE_WATCH_BACKUP, strerror(errno));
                break;
            }
        }

        __MACRO_SNPRINTF__(bpath, 0, "%s/%s", HAVE_WATCH_BACKUP, OUTDIR_DNS_REPORT);
        if (stat(bpath.str, &st) == -1)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK STR_PRN_CREATE, CP "output result directory", bpath.str);

            if (mkdir(bpath.str, 0775) == -1)
            {
                __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_CREATE, CP "output result directory", bpath.str, strerror(errno));
                break;
            }
        }

        __MACRO_SNPRINTF__(bpath, 0, "%s/%s", HAVE_WATCH_BACKUP, TFTP_FILE_BLIST);
        if (access(bpath.str, F_OK) == -1)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_NOTF, CP "block-list", bpath.str, strerror(errno));
            break;
        }
        if ((td->fi = fopen(bpath.str, "r")) == NULL)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_OPEN, CP "block-list", bpath.str, strerror(errno));
            break;
        }

        __MACRO_SNPRINTF__(bpath, 0, "%s/%s/dns-test-%lu.stat", HAVE_WATCH_BACKUP, OUTDIR_DNS_REPORT, (unsigned long)time(NULL));
        if ((td->fo = fopen(bpath.str, "w")) == NULL)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_OPEN, CP "result file", bpath.str, strerror(errno));
            break;
        }
        memcpy((void*)&reportname, (void*)bpath.str, bpath.sz);

        return 1;

    } while(0);

    return 0;
}
static void write_header(test_th_data_t *td)
{
    time_t tt = time(NULL);
    __BUFFER_CREATE__(bpath);
    __MACRO_SNPRINTF__(bpath, , "%s/%s", HAVE_WATCH_BACKUP, TFTP_FILE_UPDATE);
    __cleanup_new_fclose(FILE) fp = NULL;

    if (access(bpath.str, F_OK) == -1)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_NOTF, CP "date", bpath.str, strerror(errno));
        return;
    }
    else
    {
        if ((fp = fopen(bpath.str, "r")) == NULL)
        {
            __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_OPEN, CP "date", bpath.str, strerror(errno));
        }
    }
    if ((fp != NULL) && ((bpath.sz = fread(bpath.str, sizeof(char), __BUFFER_SIZE__, fp)) > 0))
    {

        bpath.str[bpath.sz] = '\0';
        fprintf(td->fo,
            STR_PRN_REPORT_0 STR_PRN_REPORT_1 STR_PRN_REPORT_3 STR_PRN_REPORT_4 STR_PRN_REPORT_5,
            ctime((time_t*)&tt), bpath.str
        );
    }
    else
    {
        fprintf(td->fo,
            STR_PRN_REPORT_0 STR_PRN_REPORT_2 STR_PRN_REPORT_3 STR_PRN_REPORT_4 STR_PRN_REPORT_5,
            ctime((time_t*)&tt)
        );
    }
    fflush(td->fo);
    __cleanup_notsupport_p(auto_fcloseFILE, fp);
}

static void write_footer(test_th_data_t *td)
{
    double total = (((double)(td->stop - td->start)) / CLOCKS_PER_SEC);

    if (td->fo != NULL)
    {
        fprintf(td->fo, STR_PRN_REPORT, td->cnt, td->bcnt, td->ecnt, total);
    }
    if (!isnoprn)
    {
        __MACRO_TOSCREEN__(STR_PRN_REPORT, td->cnt, td->bcnt, td->ecnt, total);
    }
}

int main(int argc, char **argv)
{
    test_th_data_t td = {0};
    string_s sline = {0};
    char *param[3] = { NULL };

    __cleanup_set_fclose(FILE, fifree, td.fi);
    __cleanup_set_fclose(FILE, fofree, td.fo);

    signal(SIGINT,  &sig_SIGINT);
    set_noprint(argc, argv);

    td.start = clock();

    if (!init_io(&td))
    {
        _exit(1);
    }
    write_header(&td);

    threadpool_t thpool = thpool_init(HAVE_SYS_CPU,"DNS-TEST");
    errno = 0;

    while ((isrun) && (getline(&sline.str, ((size_t*)&sline.sz), td.fi) != -1))
    {
        if ((sline.sz < 2) || (sline.str == NULL))
        {
            continue;
        }

        string_trim(&sline);
        param[0] = param[1] = param[2] = NULL;

        if (
            (string_split((char * const)sline.str, (char**)param, __NELE(param)) < 2) ||
            (param[0] == NULL) || (param[0][0] != 'D') || (param[1] == NULL)
           )
        {
            continue;
        }
        while (thpool_num_threads_working(thpool) == (int)(HAVE_SYS_CPU))
        {
            if (!isrun) { break; }
            sched_yield();
        }
        if (!isrun) { break; }

        td.cnt++;
        td.data.str = ((param[1][0] == '.') ? (param[1] + 1) : param[1]);
        td.data.sz  = 0;
        string_trim(&td.data);

        if (td.data.sz > 0)
        {
            test_th_data_t *tds;

            if ((tds = calloc(1, sizeof(test_th_data_t))) == NULL)
            {
                __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_ALLOC, CP "test_th_data_t", strerror(errno));
                _exit(1);
            }
            memcpy((void*)tds, (void*)&td, sizeof(test_th_data_t));
            if ((tds->data.str = calloc(1, (td.data.sz + 2))) == NULL)
            {
                __MACRO_TOSCREEN__(STR_DNS_CHECK STR_ERR_ALLOC, CP "data string", strerror(errno));
                _exit(1);
            }
            memcpy((void*)tds->data.str, (void*)td.data.str, td.data.sz);
            tds->data.sz  = td.data.sz;
            tds->lbcnt    = (unsigned long*)&td.bcnt;
            tds->lecnt    = (unsigned long*)&td.ecnt;
            thpool_add_work(thpool, th_run, (void*)tds);
        }
    }
    if (sline.str != NULL)
    {
        free(sline.str);
    }

    thpool_wait(thpool);
    thpool_destroy(thpool);

    td.stop = clock();

    write_footer(&td);
    fflush(td.fo);

    set_report_symlink();
    if (isnoprn) { fprintf(stderr, "%s", reportname); }

    return 0;
}
