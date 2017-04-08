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
wrap={{BINPATCH}}/revizor-wrapper.sh
lockfile=/var/lock/subsys/revizor-watch

# Source function library.
. /etc/rc.d/init.d/functions

backup() {
    echo "Restore BackUp blacklist rules."
    sleep 2
    cp -f {{BACKUPPATH}}/*.fw {{WATCHPATCH}}
    touch {{WATCHPATCH}}/iplist.complete
}

start() {
    if [ $UID -ne 0 ] ; then
        echo "User has insufficient privilege."
        exit 4
    fi
    [ -x $exec ] || exit 5
    [ -x $wrap ] || exit 6
    echo -n $"Starting $prog: "
    daemon $prog
    retval=$?
    echo
    [ $retval -eq 0 ] && touch $lockfile
    if [[ `echo $0 | grep -c S83` -ne 0 ]] ; then
        backup
    fi
}

stop() {
    if [ $UID -ne 0 ] ; then
        echo "User has insufficient privilege."
        exit 4
    fi
    echo -n $"Stopping $prog: "
	if [ -n "`pidof $exec`" ]; then
		killproc $exec
		RETVAL=3
	else
		failure $"Stopping $prog"
	fi
    retval=$?
    echo
    [ $retval -eq 0 ] && rm -f $lockfile
}

restart() {
    stop
    start
}

status() {
    # run checks to determine if the service is running or use generic status
    status -p /var/run/${prog}.pid ${prog}
}

case "$1" in
    start|stop|restart|status)
        $1
        ;;
    *)
        echo $"Usage: $0 {start|stop|status|restart}"
        exit 2
esac
exit $?
