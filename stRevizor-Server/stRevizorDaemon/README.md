### stRevizor
#### Registry RosKomNadzor blacklist watch daemon

+ Watch FTP/TFTP directory for the presence of RosKomNadzor registry update files and makes changes to the iptables/ipset.
+ DNS transparent filter, server part (optionals).
+ HTTP/HTTPS transparent filter to remote proxy server, support method CONNECT (optionals).
+ Blocking by IP address from iptables/ipset/user defined (ipset is optionals),
+ Small dynamic allocate memory, only hash table in use.
+ No dependency other library for base functional (optionals libexpat, iconv for parse module).

> Required:

+ Netfilter [iptables](http://netfilter.org/projects/iptables/index.html) binary package install (required).
+ Netfilter [ipset](http://netfilter.org/projects/ipset/index.html) binary package install (optionals).
+ Extended  [proxy server](http://wiki.squid-cache.org/Features/HTTPS) support method CONNECT - HTTP/HTTPS transparent filter (optionals).

for parse dump.xml:  
+ libexpat (optionals for xmlparse modules).
+ iconv (optionals for xmlparse modules).

> Filtring:

#### HTTP:
+ destenation IP address and port.
+ full URL scheme.
+ URL filter support wildcard scheme.
+ reverse IP->DNS host name (optionals).
#### HTTPS:
+ destenation IP address and port.
+ TLS SNI host name if presents (optionals).
+ reverse IP->DNS host name (optionals).
+ support icoming method CONNECT (experemintal).
#### DNS:
+ full transporent DNS UDP request.
+ DNS filter support wildcard scheme.
+ any TCP request return error 4 "NOT IMPLEMENTED" (optionals).

> Options:

First run configure, use dialog screen for set/unset [features and options](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/stRevizorDaemon/tools/README.md).   
Another way, edit manual, see [after run configure](tools/watch-config.dlg) or [source dialog options](tools/tmpl-dialog-conf.t)  


> Build step:

```shell

    ./configure
    # optional:
    make menuconfig ; ./configure --disable-win-configure
    # or:
    my-editor tools/watch-config.dlg
    ./configure  --disable-win-configure
    # next step:
    make clean; make; make install
    # run this:
     /etc/rc.d/init.d/revizor start

```

> Custom configuration step:

```shell

    # edit iptables/ipset wrapper source:
    my-editor /usr/lib/strevizor/exec/revizor-wrapper.sh

    # more event options see directory:
    /usr/lib/strevizor/exec/ipfw_*/<event name>.inc

    # edit System V5 init source:
    my-editor tools/revizor.init.t

    # next step:
    ./configure  --disable-win-configure
    make clean; make; make install

```

> Run step:

```shell

    # run this (System V5 init):
     /etc/rc.d/init.d/revizor start
    # ...
     /etc/rc.d/init.d/revizor stop
    # or correct stop all pocess
     kill -TERM `pidof revizor-watch`

    # show help:
     ./revizor-watch -h
    # show version:
     ./revizor-watch -v
    # show compiled options:
     ./revizor-watch -i
    # show environment options:
     ./revizor-watch -e

```

> Control && Debug step:

If run stRevizor in daemon mode, and needed debug or control pocess to syslog,  
set option DEBUG_SYSLOG to enable in [dialog configuration file](tools/watch-config.dlg), DEBUG_SYSLOG default not set.  
* see file: src/st-revizor-watch-debug.h  
* edit /etc/syslog.conf, see next code.

```shell

    # Check packets count to redirect DNS filter:
     iptables -t nat -L -n -v | grep <you network iface> | grep REDIRECT | grep 53

    # Check syslog message report:
     grep revizor /var/log/messages
    # or configure /etc/syslog.conf
     *.debug /var/log/boot.debug

    # Show iptables/ipset response, if debug log enable:
     cat /var/log/revizor-watch-last.log

    # Show runing process:
     ps axf | grep revizor-watch | grep -v grep
    # or show process name:
     ps -ejH | grep revizor | grep -v grep
    # Result show process name:
     14771 14771 14771 ?        00:00:00   revizor-watch
     14772 14771 14771 ?        00:00:00     revizor-watcher
     14773 14771 14771 ?        00:00:03     revizor-dnsfilt
     14774 14771 14771 ?        00:00:01     revizor-urlfilt

```

#### DNS filter test & debug

```shell

    # DNS filter UDP response check:
    # 127.0.0.1 - sample, add you run server ip or dns name
     host -a www.narkop.com 127.0.0.1
    # or
     dig +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru
    # see WARNING: 'recursion requested but not available'

    # DNS filter TCP request test,
    # always return "Host *** not found: 4(NOTIMP)"
     host -T -a yandex.ru 127.0.0.1
     dig +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru

    # DNS filter TCP AXFR request test,
    # always return "Host *** not found: 5(REFUSED)"
     dig -t AXFR +tcp +multiline +nofail @127.0.0.1 SOA yandex.ru

    # Capture DNS traffic in network:
     tcpdump -n -i eth0.xxx port 53

    # use DNS wrapper check script:
    # run on local interface 127.0.0.1
     /usr/bin/test-dns-revizor-local.sh

```

#### Example DNS filter client response (TCP)

```shell

    dig -t AXFR +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru

    ; <<>> DiG 9.9.4 <<>> -t AXFR +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru
    ; (1 server found)
    ;; global options: +cmd
    ; Transfer failed.
    ; Transfer failed.


    dig +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru

    ; <<>> DiG 9.9.4 <<>> +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru
    ; (1 server found)
    ;; global options: +cmd
    ;; Got answer:
    ;; ->>HEADER<<- opcode: QUERY, status: NOTIMP, id: 32283
    ;; flags: qr tc rd ad; QUERY: 1, ANSWER: 0, AUTHORITY: 0, ADDITIONAL: 1

    ;; OPT PSEUDOSECTION:
    ; EDNS: version: 0, flags:; udp: 4096
    ;; QUESTION SECTION:
    ;yandex.ru.             IN SOA

    ;; Query time: 2 msec
    ;; SERVER: 127.0.0.1#53(127.0.0.1)
    ;; WHEN: Sun May 14 17:22:47 MSK 2017
    ;; MSG SIZE  rcvd: 38


    host -T -a yandex.ru 127.0.0.1

    Trying "yandex.ru"
    Received 27 bytes from 127.0.0.1#53 in 49 ms
    Trying "yandex.ru"
    Using domain server:
    Name: 127.0.0.1
    Address: 127.0.0.1#53
    Aliases:
    Host yandex.ru not found: 4(NOTIMP)
    Received 27 bytes from 127.0.0.1#53 in 10 ms

```

#### DNS filter stress test

```shell

    # DNS filter utilites stress test:
    # if needed edit DNS upserver in dns-test.c source, variable REVIZOR_DNS_SERVER
    # and/or modify host list, string array dnshosts[].
     cd test/
     make clean
     make
     ./dns-test

```

* [DNS filter stress test source](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor-Server/stRevizorDaemon/test)
* [DNS wrapper check script](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor-Server/stRevizorDaemon/tools/test-dns-revizor-local.sh.t)

#### Using:

* [Thread Pool - Pithikos](https://github.com/Pithikos/C-Thread-Pool)
* [Zip library - Kuba Podgórski](https://github.com/kuba--/zip)
