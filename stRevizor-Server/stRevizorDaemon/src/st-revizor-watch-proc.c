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

FUNEXPORT long proc_runing(char *procname)
{

#   if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || (defined(__sun) && defined(__SVR4))

    struct dirent* ent;
    long lpid, rpid, xpid;
    DIR  *dir;
    FILE *fp;

    __BUFFER_CREATE__(fpath);
    __BUFFER_CREATE__(pname);

    if ((rpid = getpid()) < 0)             { return -1; }
    if ((dir  = opendir("/proc")) == NULL) { return -1; }

    while ((ent = readdir(dir)) != NULL)
    {
        lpid = atol(ent->d_name);
        if ((lpid < 0) || (lpid == rpid)) { continue; }

        __MACRO_SNPRINTF__(fpath, -1, "/proc/%ld/stat", lpid);
        if ((fp = fopen(fpath.str, "r")) != NULL)
        {
            if ((fscanf(fp, "%ld (%[^)]) %c", &xpid, pname.str, (char*)&pname.sz)) != 3 )
            {
                __MACRO_TOSYSLOG__(LOG_ERR, STR_WATCH_ISRUN, fpath.str, strerror(errno));
                fclose(fp); closedir(dir);
                return -1;
            }
            if (strstr(pname.str, procname) != NULL)
            {
                if (rpid == xpid) { continue; }
                fclose(fp); closedir(dir);
                return xpid;
            }
            fclose(fp);
        }
    }
    (void)closedir(dir);
    return -1;
#   else
    return (long)getpid();
#   endif
}
