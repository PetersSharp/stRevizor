
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

endmenu

menu "directory & locations"

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
		iptables wrapper write debug log
		default: enabled

    if WRAPPER_LOG_ENABLE

config WRAPPER_LOG_NAME
	string "iptables wrapper log name"
	default "/var/log/strevizor-wrapper-last.log"
	help
		iptables wrapper log name, write command debug output
		default: /var/log/strevizor-wrapper-last.log

    endif

endmenu

menu "Netfilter setting"

comment "stREVIZOR Netfilter setting"

config IPT_BIN
	string "iptables binary"
	default "/sbin/iptables"
	help
		(Netfilter) iptables binary file
		see debug iptables/ipset wrapper:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
		default: /sbin/iptables

config IPS_BIN
	string "ipset binary"
	default "/usr/sbin/ipset"
	help
		(Netfilter) ipset binary file
		see debug iptables/ipset wrapper:
		    https://github.com/PetersSharp/stRevizor/blob/master/stRevizor-Server/1-Way/tools/revizor-wrapper.sh
		default: /usr/sbin/ipset

config IPT_IFACE
	string "interface filtring"
	default "eth0.249"
	help
		(Netfilter) interface filtring
		default: eth0.249

config IPT_NET
	string "network witch mask"
	default "192.168.249.0/24"
	help
		(Netfilter) filtring network witch mask
		default: 192.168.249.0/24

config IPT_GW
	string "network gateway"
	default "192.168.249.66"
	help
		(Netfilter) network gateway
		default: 192.168.249.66

config IPT_FINAL
	string "final target rule"
	default "DROP"
	help
		final target rule to block chains, always DROP or REJECT
		another way its redirect to help page: REDIRECT --to 192.168.0.100:80
		or reset session: REJECT --reject-with tcp-reset
		default: DROP

endmenu

menu "DNS filter setting"

comment "stREVIZOR DNS filter setting"

config ENABLE_DNS_FILTER
	bool "DNS filter enable/disable"
	default 1
	help
		"REVIZOR DNS filter enable/disable
		default: enabled

    if ENABLE_DNS_FILTER

config DNS_CLPORT
	int "DNS client port"
	default 5300
	help
		DNS client listen port, normal mode is udp port number 53,
		if iptables redirect, use hi level port
		iptables rule to remote host:
		iptables -t nat -A PREROUTING -s 192.168.249.0/24 -i eth0.249 -p udp --dport 53 -j REDIRECT --to-ports 5300
		iptables rule to local host:
		iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
		default: 5300

config DNS_UPSRV
	string "DNS uplink server ip address"
	default "8.8.8.8"
	help
		DNS uplink server ip address, redirect to clent request
		default: 8.8.8.8

config DNS_TIMEOUT
	int "DNS query timeout, reset event (msec)"
	default 3500
	help
		DNS query timeout, reset event
		default: 3500 msec.

config DNS_LOCALR
	bool "Use DNS local iptables rule"
	default 0
	help
	iptables rule to local host:
		iptables -t nat -A OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port 5300
		default: disabled

config DNS_EMPTY_RUN
	bool "run DNS resolver is blacklist empty"
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

config WATCH_DAEMON
	bool "daemon mode"
	default 1
	help
		run as daemon mode, fork console
		default: enabled

config WATCH_SYSLOG
	bool "send error message to syslog"
	default 1
	help
		send error or statistic message to syslog
		default: enabled
