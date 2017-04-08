

# stRevizor

[![Release](https://img.shields.io/github/release/PetersSharp/stRevizor.svg?style=flat)](https://github.com/PetersSharp/stRevizor/releases/latest)
[![Issues](https://img.shields.io/github/issues/PetersSharp/stRevizor.svg?style=flat)](https://github.com/PetersSharp/stRevizor/issues)
[![License](http://img.shields.io/:license-mit-blue.svg)](https://github.com/PetersSharp/stRevizor/blob/master/LICENSE)

stRevizor - Registry RosKomNadzor blacklist converter to iptables/ipset, GUI Manager, *nix server part and more..  
* GUI Manager compatible for Mono JIT compiler version >= 3.0.4

#### Release:

* [stable version stRevizor *nix daemon: 0.0.0.9](https://github.com/PetersSharp/stRevizor/archive/0.0.0.9.tar.gz)
* [stable version stRevizor GUI Manager: 0.0.0.9](https://github.com/PetersSharp/stRevizor/releases/download/0.0.0.9/stRevizorUI-Setup-x32.exe)

#### Required local depends for GUI Manager:

* [stCore (stCore.dll)](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor/stRevizorUI/stCore-dll)
* [stCoreUI (stCoreUI.dll)](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor/stRevizorUI/stCoreUI-dll)
* [stRevizor (stRevizor.dll)](https://github.com/PetersSharp/stRevizor/tree/master/stRevizorUI/stRevizor-dll)

#### Required depends for *nix daemon:

* see [stRevizor server quick start manual]https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/stRevizorDaemon/README.md)

#### Using:

+ see [server quick start](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/stRevizorDaemon/README.md)
+ add or edit xinetd tftpd setting [tftpd.xinetd](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.xinetd)
+ put or rename tftpd map rules [tftpd.remap](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.remap)
+ run GUI Manager and update Revizor blacklist database

