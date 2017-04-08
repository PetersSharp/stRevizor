
# Build: stREVIZOR select options Menu
##### REVIZOR system setting

## Menu: Build options
##### stREVIZOR build options, see: https://github.com/PetersSharp/stRevizor

### ENABLE_SILENT
#### make source to silent output mode
##### Default value: 1

> supress full information printed to screen,
> default: is silent/quiet mode enabled

### ENABLE_DEBUGSYM
#### make source enable generate debug symbol
##### Default value: 0

> output production binary/library enable generate debug symbol,
> no strip mode.
> default: disabled

### ENABLE_USE_QSORT
#### using stdlib qsort function
##### Default value: 1

> Using stdlib qsort function
> recomended, fast sort speed
> default: enabled



## Menu: Directory and Locations
##### stREVIZOR directory & locations

### WATCH_PATH
#### TFTP/FTP directory
##### Default value: /home/tftp/192.168.220.2

> TFTP/FTP directory in which the registry data is received.
> The directory must include the IP address from which updates are made.
> example:
>     /home/tftp/IP_ADDRESS = /home/tftp/192.168.0.2
> see remap rule:
>     https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.remap
> see xinetd tftpd services:
>     https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.xinetd
> ##### ##### Default value: is wrong! edit this!

### WATCH_BACKUP
#### backup registry data to directory
##### Default value: /var/spool/strevizor

> Directory to save the previous processed registry.
> default: /var/spool/strevizor

### WATCH_DRIVER
#### driver directory, use location defined prefix
##### Default value: strevizor

> Plugin/Drivers directory, use ###ure location defined prefix
> default: ((prefix)/lib/)strevizor

### WRAPPER_LOG_ENABLE
#### iptables wrapper debug log
##### Default value: 1

> iptables wrapper write debug log to syslog
> default: enabled

### WATCH_JSON_AUTO_ENABLE
#### enable auto### in JSON format
##### Default value: 0

> Enable create auto###uration file in JSON format.
> Create file in FTP/TFTP home directory, see WATCH_PATH .
> default: disabled

### WATCH_XML_AUTO_ENABLE
#### enable auto### in XML format
##### Default value: 1

> Enable create auto###uration file in XML format.
> Create file in FTP/TFTP home directory, see WATCH_PATH .
> Warning: this file require to use GUI application stRevizor Manager
> default: enabled



## Menu: Netfilter setting
##### stREVIZOR Netfilter setting


#### Selector: Select fwall engine
##### Default value: IPFW_USER

> Prefered select fwall engine, iptables, ipset or user define
> default=IPFW_USER

### IPFW_IPT
#### (Netfilter) iptables

### IPFW_IPS
#### (Netfilter) iptables + ipset

### IPFW_IPT_BGP
#### (Netfilter) iptables + BGP Blackhole

### IPFW_IPS_BGP
#### (Netfilter) iptables + ipset + BGP Blackhole

### IPFW_USER
#### manual user defined



* ##### Begin block:  IPFW_IPT || IPFW_IPS || IPFW_IPT_BGP || IPFW_IPS_BGP

### IPFW_IPT_BIN
#### iptables binary
##### Default value: /sbin/iptables

> (Netfilter) iptables binary file
> see debug iptables/ipset wrapper:
>     https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
> default: /sbin/iptables

* ##### End block

* ##### Begin block:  IPFW_IPS || IPFW_IPS_BGP

### IPFW_IPS_BIN
#### ipset binary
##### Default value: /usr/sbin/ipset

> (Netfilter) ipset binary file
> see debug iptables/ipset wrapper:
>     https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
> default: /usr/sbin/ipset

* ##### End block

### IPFW_IFACE
#### interface filtring
##### Default value: eth0.249

> (Netfilter) interface filtring
> default: eth0.249

### IPFW_NET
#### network witch mask
##### Default value: 192.168.249.0/24

> (Netfilter) filtring network witch mask
> default: 192.168.249.0/24

### IPFW_GW
#### network gateway
##### Default value: 192.168.249.66

> (Netfilter) network gateway
> default: 192.168.249.66

### IPFW_BIND
#### Bind services filters only to network gateway interface
##### Default value: 0

> If selected, bind all services only to network gateway interface,
> see Netfilter settings, variable: IPT_GW
> Default, bind services to all network interfaces
> default: disabled

### IPFW_FINAL
#### final target rule
##### Default value: DROP

> Final target rule to block chains, always DROP or REJECT
> another way its redirect to help page: REDIRECT --to 192.168.0.100:80
> or reset session: REJECT --reject-with tcp-reset
> default: DROP

### IPFW_CLEARR
#### Clear iptables/ipset rule on start
##### Default value: 1

> Clear iptables/ipset rule from V5 init script
> default: enabled

### IPFW_DEBUGL
#### iptables/ipset debug input source list
##### Default value: 0

> iptables/ipset debug input source list
> default: disabled

### IPFW_BLOCKALL
#### enable iptables block all traffic, if operator signal send
##### Default value: 0

> enable iptables block all traffic, if operator signal send.
> Block timer start if 'iplist.dropalltimer' file receive.
> Timeout block in minutes, is body in file.
> default: disabled



## Menu: XML parser module setting
##### stREVIZOR XML parser module setting

### ENABLE_XML_PARSER
#### XML parser enable/disable
##### Default value: 1

> REVIZOR dump.xml parser enable/disable
> default: enabled

* ##### Begin block:  ENABLE_XML_PARSER



#### Selector: Round IP to NET for BGP list:
##### Default value: XML_PARSER_16

> Round IP address to netwirk address with network mask
> This address output to module event 'BGP List'
> Default round to le /16
> default=16

### XML_PARSER_32
#### NET mask /32, single host, not recommended

### XML_PARSER_24
#### NET mask /24, good list compression

### XML_PARSER_16
#### NET mask /16, default

### XML_PARSER_8
#### NET mask /8, best list compression, not recommended :)



### XML_PARSER_RESOLVE_IP_ALL
#### Resolve all IP address from domain
##### Default value: 0

> REVIZOR resolve all IP address from domain, all records (very slow)
> default: disable

### XML_PARSER_RESOLVE_IP_HTTPS
#### Resolve IP address from domain is https resource
##### Default value: 1

> REVIZOR resolve IP address from domain is https resource
> Required module URL filter is enabled
> default: enable

### XML_PARSER_BLOCK_IP_ALL
#### Block all IP address is disabled DNS/URL filter
##### Default value: 0

> REVIZOR block all IP address is disabled DNS/URL filter
> WARNING: very big data table to netfilter
> default: disable

### XML_PARSER_FIX_REVDNS
#### Add Revizot data server fixed DNS addresses
##### Default value: n02.rfc-revizor.ru

> Revizot data server fixed DNS addresses
> default: n02.rfc-revizor.ru

### XML_PARSER_FIX_WHITEIP
#### Add fixed white IP/NET addresses, comma separated list
##### Default value: 

> REVIZOR table add fixed white IP addresses, comma separated list
> default: empty

### XML_PARSER_FIX_BLACKIP
#### Add fixed black IP addresses, comma separated list
##### Default value: 

> REVIZOR table add fixed black IP addresses, comma separated list
> default: empty

### XML_PARSER_FIX_DNS
#### Add fixed domain addresses, comma separated list, wildcard dot support
##### Default value: 

> REVIZOR table add fixed domain (DNS) addresses
> comma separated list, wildcard dot support
> example: .dmain.org,myhost.net,other.host.org
> default: empty

### XML_PARSER_FIX_URL
#### Add fixed URL addresses, comma separated list, wildcard NOT support
##### Default value: 

> REVIZOR table add fixed URL addresses, comma separated list,
> this list wildcard NOT support
> example: http://www.domain.org,https://myhost.net,http://other.host.org:8080
> default: empty

### XML_PARSER_SQUID_ENABLE
#### XML parser create squid HTTP/HTTPS filter list
##### Default value: 1

> XML parser create squid HTTP/HTTPS filter list
> Create file in BackUp directory, see WATCH_BACKUP .
> default: enabled

### XML_PARSER_DEBUGI
#### XML parser routine debug info
##### Default value: 1

> XML parser routine debug messages
> default: enabled

### XML_PARSER_DEBUGA
#### XML parser all debug extended messages
##### Default value: 0

> XML parser routine all debug extended messages
> default: disabled

* ##### End block



## Menu: DNS filter setting
##### stREVIZOR DNS filter setting

### ENABLE_DNS_FILTER
#### DNS filter enable/disable
##### Default value: 1

> REVIZOR DNS filter enable/disable
> default: enabled

* ##### Begin block:  ENABLE_DNS_FILTER

### DNS_CLPORT
#### DNS client redirect to UDP/TCP port
> ##### range 1000 65536
##### Default value: 5300

> DNS client listen port, normal mode is udp port number 53,
> ##### * ##### Begin block:  iptables redirect, use hi level port
> iptables rule to remote host:
> iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p udp --dport 53 -j REDIRECT --to-ports 5300
> iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p tcp --dport 53 -j REDIRECT --to-ports 5300
> iptables rule to local host:
> iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
> iptables -t nat -A OUTPUT -o lo -p tcp --dport 53 -j REDIRECT --to-port 5300
> default: 5300

### DNS_UPSRV
#### DNS uplink server ip address
##### Default value: 8.8.8.8

> DNS uplink server ip address, redirect to clent request
> default: 8.8.8.8 (google DNS, for example)

### DNS_FILTRED_NET
#### DNS filtred network or host address
##### Default value: IPFW_NET

> DNS filtred network or host address
> default: empty, used as network witch mask (IPFW_NET)



#### Selector: Return DNS blocked A answer:
##### Default value: DNS_TARGET_NXDOMAIN

> Prefered client response code from blocked DNS request: NXDOMAIN/127.0.0.1/User input IP
> Recomendate, fast client response if record is blocked and NXDOMAIN mode set.
> default=NXDOMAIN

### DNS_TARGET_NXDOMAIN
#### return target NXDOMAIN - not found

### DNS_TARGET_LOCALHOST
#### return A answer: 127.0.0.1

### DNS_TARGET_USERSET
#### return A answer: user set IP address



* ##### Begin block:  DNS_TARGET_USERSET

### DNS_TARGET_USERIP
#### Return DNS blocked user defined IP address A answer
##### Default value: 127.0.0.1

> Return DNS blocked user defined IP address A answer,
> require enable option DNS_TARGET_USERSET
> default: 127.0.0.1

* ##### End block

### DNS_LOCALR
#### Use DNS local iptables rule
##### Default value: 0

> iptables rule to filter local host DNS traffic:
> iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
> default: disabled

### DNS_TCP_REJECT
#### Reject DNS TCP request (ANY,AXFR) to any hosts
##### Default value: 1

> reject DNS TCP request (ANY,AXFR) to any hosts
> return: 'Host *** not found: 4(NOTIMP)'
> filter ANY request test:
>   host -T -a yandex.ru 127.0.0.1
> filter AXFR request test:
>   dig -t AXFR +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru
> default: enabled

### DNS_EMPTY_RUN
#### Run DNS resolver is blacklist empty
##### Default value: 1

> run DNS resolver is blacklist empty
> default: enabled

### DNS_CLEARR
#### Clear DNS iptables rule after process
##### Default value: 1

> Clear DNS iptables rule after process,
> accept normal trace dns packet to port 53
> default: enabled

### DNS_TCPDACL_ENABLE
#### Use libwrap grant access for DNS request
##### Default value: 0

> Use libwrap grant access for DNS request, option is public inside port.
> Edit /etc/hosts.allow, add line:
>    revizor-watch: ! 192.168.249. : deny
> See tcpd manual for details.
> default: disabled

### DNS_DEBUGI
#### DNS routine debug info
##### Default value: 0

> DNS routine all debug extended messages
> default: disabled

### DNS_DEBUGP
#### DNS packet debug info
##### Default value: 0

> DNS packet debug, print in HEX all incoming and outgoing packets
> default: disabled

* ##### End block



## Menu: HTTP/HTTPS URL filter setting
##### stREVIZOR HTTP/HTTPS URL filter setting

### ENABLE_HTTP_FILTER
#### URL filter enable/disable
##### Default value: 0

> REVIZOR HTTP/HTTPS URL filter enable/disable
> default: disabled

* ##### Begin block:  ENABLE_HTTP_FILTER

### HTTP_NUMTHREAD
#### URL poll thread number
> ##### range 12 160
##### Default value: 96

> URL poll thread number, allow range 12 min, 48 max
> default: 96

### HTTP_CLPORT
#### HTTP/HTTPS client redirect to TCP port
> ##### range 1000 65536
##### Default value: 5800

> HTTP client listen port, normal mode is tcp port number 80/443,
> ##### * ##### Begin block:  iptables redirect, use hi level port
> iptables rule to remote host:
> iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
> iptables rule to local host:
> iptables -t nat -A OUTPUT -o lo -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
> default: 5800

### HTTP_FILTRED_NET
#### URL filtred network or host address
##### Default value: IPFW_NET

> HTTP/HTTPS filtred network or host address
> default: empty, used as network witch mask (IPFW_NET)

### HTTP_LOCALR
#### Use HTTP/HTTPS local iptables rule
##### Default value: 0

> iptables rule to filter local host HTTP/HTTPS traffic:
> iptables -t nat -A OUTPUT -o lo -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
> default: disabled

### HTTP_EMPTY_RUN
#### Run URL filter is blacklist empty
##### Default value: 1

> run URL filter is blacklist empty
> default: enabled

### HTTP_CLEARR
#### Clear URL iptables rule after process
##### Default value: 1

> Clear HTTP/HTTPS iptables rule after process,
> accept normal trace HTTP/HTTPS packet to destinations.
> default: enabled

### HTTP_TCPDACL_ENABLE
#### Use libwrap grant access for HTTP/HTTPS request
##### Default value: 0

> Use libwrap grant access for HTTP/HTTPS request, option is public inside port.
> Edit /etc/hosts.allow, add line:
>    revizor-watch: ! 192.168.249. : deny
> See tcpd manual for details.
> default: disabled

### HTTPS_SNI_RESOLV
#### Use TLS SNI extension to DNS resolving in HTTPS packets
##### Default value: 1

> Use TLS SNI extension to DNS resolving in HTTPS packets
> default: enabled

### HTTP_BACK_RESOLV
#### Use reverse IP to DNS resolving in HTTP packets (slow method)
##### Default value: 0

> Use reverse IP to DNS resolving in HTTP packets
> Warning! slow mode
> default: disabled

### HTTPS_BACK_RESOLV
#### Use reverse IP to DNS resolving in HTTPS packets (slow method)
##### Default value: 0

> Use reverse IP to DNS resolving in HTTPS packets
> Warning! slow mode
> default: disabled

### HTTP_ALLOW_CONNECT
#### Allow incoming CONNECT method, this turn on proxy mode (Experimental)
##### Default value: 0

> Allow incoming CONNECT method, this turn on proxy mode
> Warning! Experimental, don't use this
> default: disabled

### HTTP_DEBUGI
#### URL routine debug info
##### Default value: 0

> URL routine all debug extended messages
> default: disabled

### HTTP_DEBUGC
#### URL send/receive debug info
##### Default value: 0

> URL send/receive debug extended messages
> default: disabled

### HTTP_DEBUGP
#### HTTP/HTTPS request/response header debug info
##### Default value: 0

> HTTP/HTTPS request/response header debug information, debug only.
> default: disabled

* ##### End block



## Menu: HTTP/HTTPS Squid proxy setting
##### stREVIZOR HTTP/HTTPS Squid proxy setting

### HTTP_PROXY_HOST
#### Squid proxy server ip address
##### Default value: 127.0.0.1

> HTTP/HTTPS transparent proxy server ip address, redirect to clent request.
> If use URL filter, require extended proxy server support method CONNECT.
> default: 127.0.0.1 (local extended proxy, for example)

### HTTP_PROXY_PORT
#### HTTP proxy server port
##### Default value: 3128

> HTTP extended proxy server port, redirect to clent request.
> default: 3128 (default input proxy port for example)

### HTTP_PROXY_TPORT
#### HTTP transparent proxy server port
##### Default value: 3129

> HTTP transparent extended proxy server port, redirect to clent request.
> default: 3129 (default input proxy port for example)

### HTTPS_PROXY_TPORT
#### HTTPS transparent proxy server port
##### Default value: 3130

> HTTPS transparent extended proxy server port, redirect to clent request.
> default: 3130 (default input proxy port for example)

### HTTP_PROXY_BIN
#### Squid full path binary
##### Default value: /usr/local/squid/sbin/squid

> Squid binary location, full path include file name
> default: /usr/local/squid/sbin/squid

### HTTP_PROXY_DIR
#### Squid full path filter directory
##### Default value: /home/tftp/xxx.xxx.xxx.xxx/

> Squid filter directory location, full path
> xxx.xxx.xxx.xxx - you stRevizor server IP address
> default: /home/tftp/xxx.xxx.xxx.xxx/



* ##### Begin block:  ENABLE_DNS_FILTER || ENABLE_HTTP_FILTER

### NET_QUEUE
#### DNS/URL network queue poll size
##### Default value: 1024

> DNS/URL network queue poll for request packets
> for more information, see result 'ulimit -n' command
> default: 1024

### NET_TIMEOUT
#### DNS/URL network query timeout, wait to reset event (msec)
##### Default value: 3500

> DNS/URL network query timeout, wait to reset event (msec)
> default: 3500 msec.

* ##### End block

### WATCH_SYSLOG
#### send error message to syslog
##### Default value: 1

> send error or statistic message to syslog
> default: enabled

* ##### Begin block:  WATCH_DAEMON

### DEBUG_SYSLOG
#### send all selected debug module message to syslog (daemon mode)
##### Default value: 0

> send all selected debug module message to syslog,
> this part working in daemon mode only
> default: disabled

* ##### End block

### WATCH_DAEMON
#### daemon mode
##### Default value: 1

> run as daemon mode, fork console
> default: enabled

