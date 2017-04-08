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

RESULTYN=""
DLG_CNF_FILE="./tools/watch-config.dlg"
DLG_GCC_BIN="cc"
DLG_GCC_CFLAGS=""
DLG_GCC_LDDFLAGS=""
DLG_EXEC_RESULT=1
DLG_IPA=""

export BUILD_DIALOG_CONFIG=${DLG_CNF_FILE}

function INET_ATON() {
  local IFS=. ip num e
  ip=($1)
  for e in 3 2 1; do
    num=$(($num+${ip[$((3-$e))]}*256**$e))
  done
  num=$(($num+${ip[3]}))
  DLG_IPA=$(($num&0xFFFFFFFF))
}
function INET_NTOA() {
  local IFS=. num quad ip e
  num=$1
  for e in 3 2 1; do
    quad=$((256**$e))
    ip[$((3-$e))]=$(($num/$quad))
    num=$((num%quad))
  done
  ip[3]=${num}
  DLG_IPA="${ip[*]}"
}
function checkDialogLdflags () {
    for ext in so a dylib ; do
        for lib in ncursesw ncurses curses ; do
        ${DLG_GCC_BIN} -print-file-name=lib${lib}.${ext} | grep -q /
            if [ $? -eq 0 ]; then
                DLG_GCC_LDDFLAGS="-l${lib}"
                return
            fi
        done
    done
    ISLIB=`find /usr/lib -iname "libncurs*.so" | grep curses | head -1`
    ISLIB=`basename ${ISLIB} .so | awk '{ gsub("lib","-l",$1); print $1; }'`
    if test "${ISLIB}" != "" ;
    then
        DLG_GCC_LDDFLAGS="${ISLIB}"
    fi
}
function checkDialogCcflags () {
    for path in "" ncursesw/ ncurses/ ;
    do
        for name in ncurses curses ;
        do
            if [[ -f /usr/include/${path}${name}.h ]] ; then
                DLG_GCC_CFLAGS="-I/usr/include/${path} -DCURSES_LOC=\"${path}${name}.h\""
            elif [[ -f /usr/local/include/${path}${name}.h ]] ; then
                DLG_GCC_CFLAGS="-I/usr/local/include/${path} -DCURSES_LOC=\"${path}${name}.h\""
            fi
        done
    done
}
function buildDialogExec () {
${DLG_GCC_BIN} -std=gnu99 ${DLG_GCC_CFLAGS} -I./ -O0 ${DLG_GCC_LDDFLAGS} -o ./Build_Dialog -xc - << EOF
#include CURSES_LOC
#include "./tools/tmpl-dialog.c"
EOF
}

echo "${0}"

    if [[ "${1}" != "" ]] ;
    then
        echo -e -n $@ > ./configure.autoinstall;
    elif [[ "${0}" == "./configure" || "${0}" == "./menuconfig.sh" ]] ;
    then
        checkDialogLdflags
        checkDialogCcflags
        if test "${DLG_GCC_LDDFLAGS}" != "" && test "${DLG_GCC_CFLAGS}" != "" && test  -f ./tools/tmpl-dialog.c ;
        then
            buildDialogExec
            if [[ -f ./Build_Dialog ]] ;
            then
                ./Build_Dialog ./tools/tmpl-dialog-conf.t
                DLG_EXEC_RESULT=$?
            fi
            rm -f ./Build_Dialog
        fi
    fi

    . ${DLG_CNF_FILE}

    BUILD_DNS_BIND=${BUILD_IPFW_BIND};
    BUILD_URL_BIND=${BUILD_IPFW_BIND};

    # Return DNS blocked A answer
    if test "${BUILD_DNS_TARGET_NXDOMAIN}" == "1" ;
    then
        echo -n ""
    elif test "${BUILD_DNS_TARGET_LOCALHOST}" == "1" ;
    then
        BUILD_DNS_TARGET_NXDOMAIN="0"
        BUILD_DNS_TARGET_USERIP="127.0.0.1"
        INET_ATON ${BUILD_DNS_TARGET_USERIP}
    elif test "${BUILD_DNS_TARGET_USERSET}" == "1" ;
    then
        if test "${BUILD_DNS_TARGET_USERIP}" == "" ;
        then
            BUILD_DNS_TARGET_NXDOMAIN="1"
        else
            BUILD_DNS_TARGET_NXDOMAIN="0"
            INET_ATON ${BUILD_DNS_TARGET_USERIP}
        fi
    else
        BUILD_DNS_TARGET_NXDOMAIN="1"
    fi

    # DNS TARGET NXDOMAIN
    if test "${BUILD_DNS_TARGET_NXDOMAIN}" == "1" ;
    then
        BUILD_DNS_TARGET_USERIP="NXDOMAIN (not found)"
        BUILD_DNS_TARGET_ENDIP="0"
    else
        BUILD_DNS_TARGET_ENDIP=${DLG_IPA}UL
    fi

    # Select fwall engine enumerator
    if test "${BUILD_IPFW_IPT}" == "1" ;
    then
        BUILD_IPFW_IDX="1"
    elif test "${BUILD_IPFW_IPS}" == "1" ;
    then
        BUILD_IPFW_IDX="2"
    elif test "${BUILD_IPFW_IPT_BGP}" == "1" ;
    then
        BUILD_IPFW_IDX="3"
    elif test "${BUILD_IPFW_IPS_BGP}" == "1" ;
    then
        BUILD_IPFW_IDX="4"
    elif test "${BUILD_IPFW_USER}" == "1" ;
    then
        BUILD_IPFW_IDX="5"
    else
        BUILD_IPFW_IDX="5"
    fi

    # Select le /? netmask for XML parser
    if test "${BUILD_XML_PARSER_32}" == "1" ;
    then
        BUILD_XML_PARSER_LE="32"
    elif test "${BUILD_XML_PARSER_24}" == "1" ;
    then
        BUILD_XML_PARSER_LE="24"
    elif test "${BUILD_XML_PARSER_16}" == "1" ;
    then
        BUILD_XML_PARSER_LE="16"
    elif test "${BUILD_XML_PARSER_8}" == "1" ;
    then
        BUILD_XML_PARSER_LE="8"
    else
        BUILD_XML_PARSER_LE="16"
    fi
