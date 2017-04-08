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

    ipfw_pack_check();
    {
        __BUFFER_CREATE__(fpath);
        __MACRO_SNPRINTF__(fpath, 1, HAVE_WATCH_BACKUP "/%s", insource[IN_FILE_BLIST].str);

        if (file_exists_fp(&fpath))
        {
            ipfw_exec(IPFW_LSTSTART, NULL);
            ipfw_parse(fpath);
            ipfw_exec(IPFW_LSTEND, NULL);
            /* ipfw_stat_prn(); for debug list */
        }

#       if (defined(HAVE_XML_PARSER_SQUID_ENABLE) && (HAVE_XML_PARSER_SQUID_ENABLE == 1))
        int i, isfound = 1;
        string_s sqsource[] = {
            { __ADD_string_s(TFTP_FILE_SQUID_HTTP)  },
            { __ADD_string_s(TFTP_FILE_SQUID_HTTPS) },
            { __ADD_string_s(TFTP_FILE_SQUID_PHTTP) },
            { __ADD_string_s(TFTP_FILE_SQUID_PHTTPS)}
        };
        for (i = 0; i < __NELE(sqsource); i++)
        {
            __MACRO_SNPRINTF__(fpath, 1, HAVE_WATCH_BACKUP "/%s", sqsource[i].str);
            if (!file_exists_fp(&fpath)) { isfound = 0; break; }
        }
        if (isfound)
        {
            ipfw_exec(IPFW_SQUID, NULL);
        }
#       endif
    }
