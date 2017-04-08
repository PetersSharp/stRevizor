# stRevizor
stRevizor - Registry RosKomNadzor blacklist converter to iptables/ipset, Win GUI client, *nix server part and more..

#### Release:

* [Initial version stRevizor 0.0.0.4](https://github.com/PetersSharp/stRevizor/releases/download/0.0.0.4/stRevizorUI-Setup-x32-20170409011856.exe)

#### Required depends:

* [stCore (stCore.dll)](https://github.com/PetersSharp/stCoCServer/tree/master/stCoCServer/stExtLib/stCore-dll)
* [stCoreUI (stCoreUI.dll)](https://github.com/PetersSharp/stCoCServer/tree/master/stCoCServer/stExtLib/stCoreUI-dll)
* [stRevizor (stRevizor.dll)](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor-dll)

#### Using:

> 1 Way

+ see [1-Way, stRevizor server source](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor-Server/1-Way)

> 2 Way

+ put cron script [2-Way, revizor.iptables.sh](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/2-Way/revizor.iptables.sh) to bin or other directory
+ add or edit xinetd tftpd setting [2-Way, tftpd.xinetd](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.xinetd)
+ put or rename tftpd map rules [2-Way, tftpd.remap](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.remap)
+ add to cron job rules, see example [2-Way, revizor.cron](https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/2-Way/revizor.cron)

> 1 or 2 Way

+ run Win GUI client and update Revizor blacklist database

