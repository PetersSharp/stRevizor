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

    if (access(HAVE_WATCH_DRIVER, F_OK) == -1)
    {
        if (mkdir(HAVE_WATCH_DRIVER, 0775) == -1)
        {
            __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: lib directory %s -> %s",
                HAVE_WATCH_DRIVER,
                strerror(errno)
            );
            _exit(2);
        }
    }
    chdir(HAVE_WATCH_DRIVER);
