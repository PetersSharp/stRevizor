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

void file_clears(string_s *fdir, const string_s *arr, int asz)
{
    DIR *directory;
    struct dirent* file;

    if ((directory = opendir(fdir->str)) != NULL) {
        while ((file = readdir(directory)) != NULL) {
            if (file->d_type == DT_REG)
            {
                for (int i = 0; i < asz; i++)
                {
                    if (memcmp(arr[i].str, file->d_name, arr[i].sz) == 0)
                    {
                        switch(i)
                        {
                            case IN_FILE_COMPLETE:
                            case IN_FILE_PROCESS:
                            case IN_FILE_BTIMER:
                            {
                                __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Input file: %s -> Delete", arr[i].str);
                                file_delete(fdir, (string_s*)&arr[i]);
                                break;
                            }
                            case IN_FILE_BLIST:
                            case IN_FILE_PLIST:
                            case IN_FILE_BLXML:
                            case IN_FILE_BLZIP:
                            {
                                __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Input file: %s -> BackUp", arr[i].str);
                                file_backup(fdir, (string_s*)&arr[i]);
                                break;
                            }
                            default:
                            {
                                __MACRO_TOSYSLOG__(LOG_INFO, STR_WATCH_DEBUG "Input file: %s -> Skip", arr[i].str);
                                // file_backup(fdir, (string_s*)&arr[i]);
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
int file_exists(string_s *fdir, string_s *fname)
{
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, 0, "%s/%s", fdir->str, fname->str);
    return file_exists_fp(&fpath);
}
int file_exists_fp(string_s *fpath)
{
    if (access(fpath->str, F_OK) != -1)
    {
        return 1;
    }
    return 0;
}
void file_delete(string_s *fdir, string_s *fname)
{
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir->str, fname->str);
    file_delete_fp(&fpath);
}
void file_delete_fp(string_s *fpath)
{
    if (remove(fpath->str) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Unable to delete %s, %s", fpath->str, strerror(errno));
        errno = 0;
    }
}
void file_create(string_s *fdir, const string_s *fname, string_s *content)
{
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir->str, fname->str);

    file_create_fp(&fpath, content);
}
void file_create_fp(string_s *fpath, string_s *content)
{
    int fd;
    if ((fd = open(fpath->str, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Not open file:  %s (%s)", strerror(errno), fpath->str);
        exit(2);
    }
    if (content != NULL)
    {
        write(fd, content->str, content->sz);
    }
    close(fd);
}
static void file_backup_(string_s *fpath, string_s *opath)
{
    struct stat st = {0};
    if (stat(HAVE_WATCH_BACKUP, &st) == -1)
    {
        if (mkdir(HAVE_WATCH_BACKUP, 0775) == -1)
        {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Unable create backup directory %s, %s", HAVE_WATCH_BACKUP, strerror(errno));
        errno = 0;
        return;
        }
    }
    if (rename(fpath->str, opath->str) == -1)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ERROR "Unable to move %s, %s", fpath->str, strerror(errno));
        errno = 0;
    }
}
void file_backup(string_s *fdir, string_s *fname)
{
    __BUFFER_CREATE__(fpath);
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir->str, fname->str);
    __MACRO_SNPRINTF__(opath, , "%s/%s", HAVE_WATCH_BACKUP, fname->str);
    file_backup_(&fpath, &opath);
}
void file_backup_fp(string_s *fpath, const string_s *arr, int n)
{
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(opath, , "%s/%s", HAVE_WATCH_BACKUP, arr[n].str);
    file_backup_(fpath, &opath);
}
void file_move(string_s *fdir, string_s *sname, string_s *oname)
{
    __BUFFER_CREATE__(fpath);
    __BUFFER_CREATE__(opath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", fdir->str, sname->str);
    __MACRO_SNPRINTF__(opath, , "%s/%s", fdir->str, oname->str);
    file_backup_(&fpath, &opath);
}
void file_move_fp(string_s *spath, string_s *opath)
{
    file_backup_(spath, opath);
}
void file_read(string_s *fpath, fwall_cb fcb)
{
    int cnt = 0;
    FILE *fp;
    string_s data = {0};

    if ((fp = fopen(fpath->str, "r")) != NULL)
    {
        while (getline(&data.str, ((size_t*)&data.sz), fp) != -1)
        {
            string_trim(&data);
            __MACRO_VULNERABILITY__(data);
#           if defined(HAVE_IPT_DEBUGL) && (HAVE_IPT_DEBUGL == 1)
            __MACRO_TOSCREEN__("\t[%d] -> %s\t\t(%s)", cnt, data.str, fpath.str);
#           endif
            fcb(&data);
            cnt++;
        }
        if (data.str != NULL)
        {
            free(data.str);
        }
        fclose(fp);
        __MACRO_TOSYSLOG__(LOG_INFO, STR_IPT_UPDATE ", count address: %d -> %s", "end update", cnt, fpath->str);
    }
}
long file_size(FILE *fp)
{
    do {
        if (fp == NULL)                   { break; }
        long c, p;
        if ((p = ftell(fp)) == -1)        { break; }
        if (fseek(fp, 0L, SEEK_END) != 0) { break; }
        if ((c = ftell(fp)) == -1)        { break; }
        if (fseek(fp, p, SEEK_SET) != 0)  { break; }
        return c;

    } while(0);

    return -1;
}
