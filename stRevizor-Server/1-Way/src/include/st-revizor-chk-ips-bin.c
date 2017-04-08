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

    if (access(HAVE_IPS_BIN, F_OK) != -1)
    {
        __SET_string_s(binpath.ips, HAVE_IPS_BIN);
    }
    else if (access(HAVE_FIND_IPS_BIN, F_OK) != -1)
    {
        __SET_string_s(binpath.ips, HAVE_FIND_IPS_BIN);
    }
    else
    {
        __MACRO_TOSYSLOG__(LOG_ERR, "[Fatal Error]: (Netfilter) file: (%s or %s), %s",
            HAVE_IPS_BIN,
            HAVE_FIND_IPS_BIN,
            strerror(errno)
        );
    }
