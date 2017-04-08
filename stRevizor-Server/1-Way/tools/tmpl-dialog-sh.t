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

export BUILD_DIALOG_CONFIG=${DLG_CNF_FILE}

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
