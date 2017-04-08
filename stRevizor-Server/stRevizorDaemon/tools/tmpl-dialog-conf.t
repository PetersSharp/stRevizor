
mainmenu "stREVIZOR select options Menu"
comment "REVIZOR system setting"

menu "Build options"
comment "stREVIZOR build options, see: https://github.com/PetersSharp/stRevizor"

config ENABLE_SILENT
	bool "make source to silent output mode"
	default 1
	help
		supress full information printed to screen,
		default: is silent/quiet mode enabled

config ENABLE_DEBUGSYM
	bool "make source enable generate debug symbol"
	default 0
	help
		output production binary/library enable generate debug symbol,
		no strip mode.
		default: disabled

config ENABLE_USE_QSORT
	bool "using stdlib qsort function"
	default 1
	help
		Using stdlib qsort function
		recomended, fast sort speed
		default: enabled

endmenu

menu "Directory and Locations"
comment "stREVIZOR directory & locations"

config WATCH_PATH
	string "TFTP/FTP directory"
	default "/home/tftp/192.168.220.2"
	help
		TFTP/FTP directory in which the registry data is received.
		The directory must include the IP address from which updates are made.
		example:
		    /home/tftp/IP_ADDRESS = /home/tftp/192.168.0.2
		see remap rule:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.remap
		see xinetd tftpd services:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/tftpd-settings/tftpd.xinetd
		default is wrong! edit this!

config WATCH_BACKUP
	string "backup registry data to directory"
	default "/var/spool/strevizor"
	help
		Directory to save the previous processed registry.
		default: /var/spool/strevizor

config WATCH_DRIVER
	string "driver directory, use location defined prefix"
	default "strevizor"
	help
		Plugin/Drivers directory, use configure location defined prefix
		default: ((prefix)/lib/)strevizor

config WRAPPER_LOG_ENABLE
	bool "iptables wrapper debug log"
	default 1
	help
		iptables wrapper write debug log to syslog
		default: enabled

config WATCH_JSON_AUTO_ENABLE
	bool "enable autoconfig in JSON format"
	default 0
	help
		Enable create autoconfiguration file in JSON format.
		Create file in FTP/TFTP home directory, see WATCH_PATH .
		default: disabled

config WATCH_XML_AUTO_ENABLE
	bool "enable autoconfig in XML format"
	default 1
	help
		Enable create autoconfiguration file in XML format.
		Create file in FTP/TFTP home directory, see WATCH_PATH .
		Warning: this file require to use GUI application stRevizor Manager
		default: enabled

endmenu

menu "Netfilter setting"
comment "stREVIZOR Netfilter setting"

choice
	prompt "Select fwall engine"
	default IPFW_USER
	help
		Prefered select fwall engine, iptables, ipset or user define
		default=IPFW_USER

	config IPFW_IPT
	bool "(Netfilter) iptables"

	config IPFW_IPS
	bool "(Netfilter) iptables + ipset"

	config IPFW_IPT_BGP
	bool "(Netfilter) iptables + BGP Blackhole"

	config IPFW_IPS_BGP
	bool "(Netfilter) iptables + ipset + BGP Blackhole"

	config IPFW_USER
	bool "manual user defined"

endchoice

	if IPFW_IPT || IPFW_IPS || IPFW_IPT_BGP || IPFW_IPS_BGP

config IPFW_IPT_BIN
	string "iptables binary"
	default "/sbin/iptables"
	help
		(Netfilter) iptables binary file
		see debug iptables/ipset wrapper:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
		default: /sbin/iptables

	endif

	if IPFW_IPS || IPFW_IPS_BGP

config IPFW_IPS_BIN
	string "ipset binary"
	default "/usr/sbin/ipset"
	help
		(Netfilter) ipset binary file
		see debug iptables/ipset wrapper:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
		default: /usr/sbin/ipset

	endif

config IPFW_IFACE
	string "interface filtring"
	default "eth0.249"
	help
		(Netfilter) interface filtring
		default: eth0.249

config IPFW_NET
	string "network witch mask"
	default "192.168.249.0/24"
	help
		(Netfilter) filtring network witch mask
		default: 192.168.249.0/24

config IPFW_GW
	string "network gateway"
	default "192.168.249.66"
	help
		(Netfilter) network gateway
		default: 192.168.249.66

config IPFW_BIND
	bool "Bind services filters only to network gateway interface"
	default 0
	help
		If selected, bind all services only to network gateway interface,
		see Netfilter settings, variable: IPT_GW
		Default, bind services to all network interfaces
		default: disabled

config IPFW_FINAL
	string "final target rule"
	default "DROP"
	help
		Final target rule to block chains, always DROP or REJECT
		another way its redirect to help page: REDIRECT --to 192.168.0.100:80
		or reset session: REJECT --reject-with tcp-reset
		default: DROP

config IPFW_CLEARR
	bool "Clear iptables/ipset rule on start"
	default 1
	help
		Clear iptables/ipset rule from V5 init script
		default: enabled

config IPFW_DEBUGL
	bool "iptables/ipset debug input source list"
	default 0
	help
		iptables/ipset debug input source list
		default: disabled

config IPFW_BLOCKALL
	bool "enable iptables block all traffic, if operator signal send"
	default 0
	help
		enable iptables block all traffic, if operator signal send.
		Block timer start if 'iplist.dropalltimer' file receive.
		Timeout block in minutes, is body in file.
		default: disabled

endmenu

menu "XML parser module setting"
comment "stREVIZOR XML parser module setting"

config ENABLE_XML_PARSER
	bool "XML parser enable/disable"
	default 1
	help
		REVIZOR dump.xml parser enable/disable
		default: enabled

	if ENABLE_XML_PARSER

choice

	prompt "Round IP to NET for BGP list:"
	default XML_PARSER_16
	help
		Round IP address to netwirk address with network mask
		This address output to module event 'BGP List'
		Default round to le /16
		default=16

	config XML_PARSER_32
	bool "NET mask /32, single host, not recommended"

	config XML_PARSER_24
	bool "NET mask /24, good list compression"

	config XML_PARSER_16
	bool "NET mask /16, default"

	config XML_PARSER_8
	bool "NET mask /8, best list compression, not recommended :)"

endchoice

config XML_PARSER_RESOLVE_IP_ALL
	bool "Resolve all IP address from domain"
	default 0
	help
		REVIZOR resolve all IP address from domain, all records (very slow)
		default: disable

config XML_PARSER_RESOLVE_IP_HTTPS
	bool "Resolve IP address from domain is https resource"
	default 1
	help
		REVIZOR resolve IP address from domain is https resource
		Required module URL filter is enabled
		default: enable

config XML_PARSER_BLOCK_IP_ALL
	bool "Block all IP address is disabled DNS/URL filter"
	default 0
	help
		REVIZOR block all IP address is disabled DNS/URL filter
		WARNING: very big data table to netfilter
		default: disable

config XML_PARSER_FIX_REVDNS
	string "Add Revizot data server fixed DNS addresses"
	default "n02.rfc-revizor.ru"
	help
		Revizot data server fixed DNS addresses
		default: n02.rfc-revizor.ru

config XML_PARSER_FIX_WHITEIP
	string "Add fixed white IP/NET addresses, comma separated list"
	default ""
	help
		REVIZOR table add fixed white IP addresses, comma separated list
		default: empty

config XML_PARSER_FIX_BLACKIP
	string "Add fixed black IP addresses, comma separated list"
	default ""
	help
		REVIZOR table add fixed black IP addresses, comma separated list
		default: empty

config XML_PARSER_FIX_DNS
	string "Add fixed domain addresses, comma separated list, wildcard dot support"
	default ""
	help
		REVIZOR table add fixed domain (DNS) addresses
		comma separated list, wildcard dot support
		example: .dmain.org,myhost.net,other.host.org
		default: empty

config XML_PARSER_FIX_URL
	string "Add fixed URL addresses, comma separated list, wildcard NOT support"
	default ""
	help
		REVIZOR table add fixed URL addresses, comma separated list,
		this list wildcard NOT support
		example: http://www.domain.org,https://myhost.net,http://other.host.org:8080
		default: empty

config XML_PARSER_SQUID_ENABLE
	bool "XML parser create squid HTTP/HTTPS filter list"
	default 1
	help
		XML parser create squid HTTP/HTTPS filter list
		Create file in BackUp directory, see WATCH_BACKUP .
		default: enabled

config XML_PARSER_DEBUGI
	bool "XML parser routine debug info"
	default 1
	help
		XML parser routine debug messages
		default: enabled

config XML_PARSER_DEBUGA
	bool "XML parser all debug extended messages"
	default 0
	help
		XML parser routine all debug extended messages
		default: disabled

	endif

endmenu

menu "DNS filter setting"
comment "stREVIZOR DNS filter setting"

config ENABLE_DNS_FILTER
	bool "DNS filter enable/disable"
	default 1
	help
		REVIZOR DNS filter enable/disable
		default: enabled

	if ENABLE_DNS_FILTER

config DNS_CLPORT
	int "DNS client redirect to UDP/TCP port"
	range 1000 65536
	default 5300
	help
		DNS client listen port, normal mode is udp port number 53,
		if iptables redirect, use hi level port
		iptables rule to remote host:
		iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p udp --dport 53 -j REDIRECT --to-ports 5300
		iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p tcp --dport 53 -j REDIRECT --to-ports 5300
		iptables rule to local host:
		iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
		iptables -t nat -A OUTPUT -o lo -p tcp --dport 53 -j REDIRECT --to-port 5300
		default: 5300

config DNS_UPSRV
	string "DNS uplink server ip address"
	default "8.8.8.8"
	help
		DNS uplink server ip address, redirect to clent request
		default: 8.8.8.8 (google DNS, for example)

config DNS_FILTRED_NET
	string "DNS filtred network or host address"
	default IPFW_NET
	help
		DNS filtred network or host address
		default: empty, used as network witch mask (IPFW_NET)

choice

	prompt "Return DNS blocked A answer:"
	default DNS_TARGET_NXDOMAIN
	help
		Prefered client response code from blocked DNS request: NXDOMAIN/127.0.0.1/User input IP
		Recomendate, fast client response if record is blocked and NXDOMAIN mode set.
		default=NXDOMAIN

	config DNS_TARGET_NXDOMAIN
	bool "return target NXDOMAIN - not found"

	config DNS_TARGET_LOCALHOST
	bool "return A answer: 127.0.0.1"

	config DNS_TARGET_USERSET
	bool "return A answer: user set IP address"

endchoice

	if DNS_TARGET_USERSET

config DNS_TARGET_USERIP
	string "Return DNS blocked user defined IP address A answer"
	default "127.0.0.1"
	help
		Return DNS blocked user defined IP address A answer,
		require enable option DNS_TARGET_USERSET
		default: 127.0.0.1

	endif

config DNS_LOCALR
	bool "Use DNS local iptables rule"
	default 0
	help
		iptables rule to filter local host DNS traffic:
		iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
		default: disabled

config DNS_TCP_REJECT
	bool "Reject DNS TCP request (ANY,AXFR) to any hosts"
	default 1
	help
		reject DNS TCP request (ANY,AXFR) to any hosts
		return: 'Host *** not found: 4(NOTIMP)'
		filter ANY request test:
		  host -T -a yandex.ru 127.0.0.1
		filter AXFR request test:
		  dig -t AXFR +tcp +multiline +nofail +besteffort @127.0.0.1 SOA yandex.ru
		default: enabled

config DNS_EMPTY_RUN
	bool "Run DNS resolver is blacklist empty"
	default 1
	help
		run DNS resolver is blacklist empty
		default: enabled

config DNS_CLEARR
	bool "Clear DNS iptables rule after process"
	default 1
	help
		Clear DNS iptables rule after process,
		accept normal trace dns packet to port 53
		default: enabled

config DNS_TCPDACL_ENABLE
	bool "Use libwrap grant access for DNS request"
	default 0
	help
		Use libwrap grant access for DNS request, option is public inside port.
		Edit /etc/hosts.allow, add line:
		   revizor-watch: ! 192.168.249. : deny
		See tcpd manual for details.
		default: disabled

config DNS_DEBUGI
	bool "DNS routine debug info"
	default 0
	help
		DNS routine all debug extended messages
		default: disabled

config DNS_DEBUGP
	bool "DNS packet debug info"
	default 0
	help
		DNS packet debug, print in HEX all incoming and outgoing packets
		default: disabled

	endif

endmenu

menu "HTTP/HTTPS URL filter setting"
comment "stREVIZOR HTTP/HTTPS URL filter setting"

config ENABLE_HTTP_FILTER
	bool "URL filter enable/disable"
	default 0
	help
		REVIZOR HTTP/HTTPS URL filter enable/disable
		default: disabled

	if ENABLE_HTTP_FILTER

config HTTP_NUMTHREAD
	int "URL poll thread number"
	range 12 160
	default 96
	help
		URL poll thread number, allow range 12 min, 48 max
		default: 96

config HTTP_CLPORT
	int "HTTP/HTTPS client redirect to TCP port"
	range 1000 65536
	default 5800
	help
		HTTP client listen port, normal mode is tcp port number 80/443,
		if iptables redirect, use hi level port
		iptables rule to remote host:
		iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
		iptables rule to local host:
		iptables -t nat -A OUTPUT -o lo -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
		default: 5800

config HTTP_FILTRED_NET
	string "URL filtred network or host address"
	default IPFW_NET
	help
		HTTP/HTTPS filtred network or host address
		default: empty, used as network witch mask (IPFW_NET)

config HTTP_LOCALR
	bool "Use HTTP/HTTPS local iptables rule"
	default 0
	help
		iptables rule to filter local host HTTP/HTTPS traffic:
		iptables -t nat -A OUTPUT -o lo -p tcp -i eth0 -m multiport --dports 80,88,...,443 -j REDIRECT --to-ports 5800
		default: disabled

config HTTP_EMPTY_RUN
	bool "Run URL filter is blacklist empty"
	default 1
	help
		run URL filter is blacklist empty
		default: enabled

config HTTP_CLEARR
	bool "Clear URL iptables rule after process"
	default 1
	help
		Clear HTTP/HTTPS iptables rule after process,
		accept normal trace HTTP/HTTPS packet to destinations.
		default: enabled

config HTTP_TCPDACL_ENABLE
	bool "Use libwrap grant access for HTTP/HTTPS request"
	default 0
	help
		Use libwrap grant access for HTTP/HTTPS request, option is public inside port.
		Edit /etc/hosts.allow, add line:
		   revizor-watch: ! 192.168.249. : deny
		See tcpd manual for details.
		default: disabled

config HTTPS_SNI_RESOLV
	bool "Use TLS SNI extension to DNS resolving in HTTPS packets"
	default 1
	help
		Use TLS SNI extension to DNS resolving in HTTPS packets
		default: enabled

config HTTP_BACK_RESOLV
	bool "Use reverse IP to DNS resolving in HTTP packets (slow method)"
	default 0
	help
		Use reverse IP to DNS resolving in HTTP packets
		Warning! slow mode
		default: disabled

config HTTPS_BACK_RESOLV
	bool "Use reverse IP to DNS resolving in HTTPS packets (slow method)"
	default 0
	help
		Use reverse IP to DNS resolving in HTTPS packets
		Warning! slow mode
		default: disabled

config HTTP_ALLOW_CONNECT
	bool "Allow incoming CONNECT method, this turn on proxy mode (Experimental)"
	default 0
	help
		Allow incoming CONNECT method, this turn on proxy mode
		Warning! Experimental, don't use this
		default: disabled

config HTTP_DEBUGI
	bool "URL routine debug info"
	default 0
	help
		URL routine all debug extended messages
		default: disabled

config HTTP_DEBUGC
	bool "URL send/receive debug info"
	default 0
	help
		URL send/receive debug extended messages
		default: disabled

config HTTP_DEBUGP
	bool "HTTP/HTTPS request/response header debug info"
	default 0
	help
		HTTP/HTTPS request/response header debug information, debug only.
		default: disabled

	endif

endmenu

menu "HTTP/HTTPS Squid proxy setting"
comment "stREVIZOR HTTP/HTTPS Squid proxy setting"

config HTTP_PROXY_HOST
	string "Squid proxy server ip address"
	default "127.0.0.1"
	help
		HTTP/HTTPS transparent proxy server ip address, redirect to clent request.
		If use URL filter, require extended proxy server support method CONNECT.
		default: 127.0.0.1 (local extended proxy, for example)

config HTTP_PROXY_PORT
	int "HTTP proxy server port"
	default 3128
	help
		HTTP extended proxy server port, redirect to clent request.
		default: 3128 (default input proxy port for example)

config HTTP_PROXY_TPORT
	int "HTTP transparent proxy server port"
	default 3129
	help
		HTTP transparent extended proxy server port, redirect to clent request.
		default: 3129 (default input proxy port for example)

config HTTPS_PROXY_TPORT
	int "HTTPS transparent proxy server port"
	default 3130
	help
		HTTPS transparent extended proxy server port, redirect to clent request.
		default: 3130 (default input proxy port for example)

config HTTP_PROXY_BIN
	string "Squid full path binary"
	default "/usr/local/squid/sbin/squid"
	help
		Squid binary location, full path include file name
		default: /usr/local/squid/sbin/squid

config HTTP_PROXY_DIR
	string "Squid full path filter directory"
	default "/home/tftp/xxx.xxx.xxx.xxx/"
	help
		Squid filter directory location, full path
		xxx.xxx.xxx.xxx - you stRevizor server IP address
		default: /home/tftp/xxx.xxx.xxx.xxx/

endmenu

	if ENABLE_DNS_FILTER || ENABLE_HTTP_FILTER

config NET_QUEUE
	int "DNS/URL network queue poll size"
	default 1024
	help
		DNS/URL network queue poll for request packets
		for more information, see result 'ulimit -n' command
		default: 1024

config NET_TIMEOUT
	int "DNS/URL network query timeout, wait to reset event (msec)"
	default 3500
	help
		DNS/URL network query timeout, wait to reset event (msec)
		default: 3500 msec.

	endif

config WATCH_SYSLOG
	bool "send error message to syslog"
	default 1
	help
		send error or statistic message to syslog
		default: enabled

	if WATCH_DAEMON

config DEBUG_SYSLOG
	bool "send all selected debug module message to syslog (daemon mode)"
	default 0
	help
		send all selected debug module message to syslog,
		this part working in daemon mode only
		default: disabled

	endif

config WATCH_DAEMON
	bool "daemon mode"
	default 1
	help
		run as daemon mode, fork console
		default: enabled

