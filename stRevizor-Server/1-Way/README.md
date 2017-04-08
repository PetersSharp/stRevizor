### stRevizor
#### Registry RosKomNadzor blacklist watch daemon

+ Watch FTP/TFTP directory for the presence of RosKomNadzor registry update files and makes changes to the iptables/ipset.
+ DNS trasporent filter - optionals.

> Options:

First run configure, use dialog screen for set/unset features and options.  
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
    my-editor tools/revizor-wrapper.sh.t

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

```

