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

# This script will be executed to set iptables/ipset rules.
# $1 - ipset bin or empty or white space
# $2 - ipset bin or empty or white space
# $3 - iptables or ipset command line

LOG_DEBUG_PATH="{{LOGNAME}}"
LOG_DEBUG_ENABLE={{LOGENABLE}}
IPT_BIN=""

    if [[ ${1} != "" && ${1} != " " ]] ; then
        if [[ ${LOG_DEBUG_ENABLE} == 1 ]] ; then
            echo -e -n " SRC: [${1}] ${3}\n\t" >>${LOG_DEBUG_PATH}
            ${1} -v ${3} >>${LOG_DEBUG_PATH} 2>&1
        else
            ${1} ${3} >>${LOG_DEBUG_PATH} 2>&1
        fi
    fi
    if [[ ${2} != "" && ${2} != " " ]] ; then
        if [[ ${LOG_DEBUG_ENABLE} == 1 ]] ; then
            echo -e -n " SRC: [${2}] ${3}\n\t" >>${LOG_DEBUG_PATH}
            ${2} ${3} >>${LOG_DEBUG_PATH} 2>&1
        else
            ${2} -q ${3} >>${LOG_DEBUG_PATH} 2>&1
        fi
    fi
