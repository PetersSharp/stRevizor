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
// watchers child

    if (wrun.run != NULL)
    {
        for (i = 0; ((i < wrun.cnt) || (i == 0)); i++)
        {
            if (wrun.run[i] != NULL)
            {
                __MACRO_WAITPID__(wrun.run[i]->pid, wrun.run[i]->ret, wrun.run[i]->dir.str, "Watcher");
#               if !defined(__GNUC__) && !defined(__GCC__)
                free(wrun.run[i]);
#               endif
            }
        }
#       if !defined(__GNUC__) && !defined(__GCC__)
        free(wrun.run);
        wrun.run = NULL;
#       endif
    }
