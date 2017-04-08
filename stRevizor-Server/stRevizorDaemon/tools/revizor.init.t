#!/bin/sh
#
# revizor-watch  Start/Stop Revizor watcher.
#
# chkconfig: 2345 83 83
# description: stRevizor -  Registry RosKomNadzor blacklist watch daemon \
#              Watch FTP/TFTP directory for the presence of RosKomNadzor \
#              registry update files and makes changes to the iptables/ipset. \
#              see: https://github.com/PetersSharp/stRevizor

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


RETVAL=0
prog="revizor-watch"
exec={{BINPATCH}}/revizor-watch
iface={{NETIFACE}}
execipt={{IPTBIN}}
execips={{IPSBIN}}
isclear={{IPTCLEAR}}
lockfile=/var/lock/subsys/revizor-watch

# Source function library.
. /etc/rc.d/init.d/functions

cleanipt() {
    if [[ ${execipt} != "" && ${isclear} -eq 1 ]] ;
    then
        echo "Clean iptables blacklist rules."
        for i in `${execipt} -L REVIZOR -n | grep -Po 'REV[^ ]*'` "REVIZOR" "REVIZORBLACK" "REVIZORWHITE" "REVIZORBGP"
        do
            ${execipt} -F ${i} 2>&1
            ${execipt} -X ${i} 2>&1
        done
    fi
    if [[ ${execips} != "" && ${isclear} -eq 1 ]] ;
    then
        echo "Clean ipset blacklist rules."
        for i in `${execips} -L -n | grep REVIZOR`
        do
            ${execips} -F ${i} 2>&1
            ${execips} -X ${i} 2>&1
        done
    fi
}

startrev() {
    if [ $UID -ne 0 ] ; then
        echo "User has insufficient privilege, needed root access."
        exit 4
    fi
    [ -x $exec ] || exit 5
    echo -n $"Starting $prog: "
    daemon $prog
    retval=$?
    echo
    [ $retval -eq 0 ] && touch $lockfile
}

stoprev() {
    if [ $UID -ne 0 ] ; then
        echo "User has insufficient privilege."
        exit 4
    fi
    echo -n $"Stopping $prog: "
	if [ -n "`pidof $exec`" ]; then
		# not work property, TODO: fix this
		# killproc $exec
		kill -TERM `pidof ${prog}`
		RETVAL=3
	else
		failure $"Stopping $prog"
	fi
    retval=$?
    echo
    [ $retval -eq 0 ] && rm -f $lockfile
}

start() {
    startrev
}

stop() {
    stoprev
}

restart() {
    stoprev
    cleanipt
    startrev
}

clean() {
    isclear=1
    cleanipt
}

build() {
    ${exec} -i
    ${exec} -e
    echo " - Location revizor-watch binary: ${exec}"
}

trcedns() {
    if [[ ${iface} != "" ]] ;
    then
        tcpdump -n -i ${iface} port 53
    fi
}

trcehttp() {
    if [[ ${iface} != "" ]] ;
    then
        tcpdump -n -i ${iface} port 80
    fi
}

status() {
    # run checks to determine if the service is running or use generic status
    status -p /var/run/${prog}.pid ${prog}
}

case "$1" in
    start|stop|restart|clean|build|tracedns|trcehttp|status)
        $1
        ;;
    *)
        echo $"Usage: $0 {start|stop|restart|clean|build|tracedns|trcehttp|status}"
        exit 2
esac
exit $?
