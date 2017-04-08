#!/bin/bash

#--------------------------------------------------------------------------------------
#
#    stRevizor package - Registry RosKomNadzor blacklist converter to iptables/ipset,
#              Win GUI client, *nix server part and more..
#
#    stRevizor server - Registry RosKomNadzor blacklist watch daemon.
#              Watch FTP/TFTP directory for the presence of RosKomNadzor
#              registry update files and makes changes to the iptables/ipset.
#
#              Copyright (C) 2015-2017, @PS
#
#    Everyone is permitted to copy and distribute verbatim copies
#    of this license document, but changing it is not allowed.
#
#    See file LICENSE.md in directory distribution.
#
#    You can find latest source:
#     - https://github.com/PetersSharp/stRevizor
#     - https://github.com/PetersSharp/stRevizor/releases/latest
#     - https://github.com/PetersSharp/stRevizor/pulse
#
#    * $Id$
#    * commit date: $Format:%cd by %aN$
#
#--------------------------------------------------------------------------------------

EXEC="revizor-watch"

valgrind \
--leak-check=full \
--show-reachable=yes \
--track-origins=yes \
--vgdb=yes \
--error-limit=no \
--log-file="logfile.out" \
-v .build/revizor-watch/${EXEC}

echo -e "\n\tLDD link library: ${EXEC}\n"
ldd .build/revizor-watch/${EXEC}

