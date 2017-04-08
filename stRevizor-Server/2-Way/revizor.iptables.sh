#!/bin/bash

# -- Settings --

REVIZOR_IFACE="eth0.2"
REVIZOR_NET="192.168.0.0/24"
REVIZOR_GW="192.168.0.1"

DIRSOURCE="/home/tftp/192.168.0.2/"
DIRBACKUP="/home/backup/RevizorBackUp/"

# -- Variables --

CHAIN_MAIN="REVIZOR"
CHAIN_IP="REVIZORIP"
CHAIN_URL="REVIZORURL"
CHARULERASE="-F -X"
IPRULE="-D -I"
TARGET="INPUT FORWARD"
IPSOURCE="iplist.fw"
URLSOURCE="urllist.fw"
WHIESOURCE="whitelist.fw"
FLAGBUSY="iplist.proccess"
FLAGLOAD="iplist.complete"

# -- Code --

    cd ${DIRSOURCE}

    if [[ -f ${DIRSOURCE}${FLAGBUSY} ]] ; then
	exit
    fi
    if [[ ! -f ${DIRSOURCE}${FLAGLOAD} ]] ; then
	exit
    fi

    touch ${DIRSOURCE}${FLAGBUSY}

    for src in ${IPSOURCE} ${URLSOURCE} ${WHIESOURCE}
    do
        if [[ -f ${src} ]] ; then
            dos2unix ${src}
        fi
    done

    for cha in ${CHAIN_MAIN} ${CHAIN_IP} ${CHAIN_URL}
    do
        iptables -t filter -N ${cha}TMP
    done

    if [[ -f ${DIRSOURCE}${WHIESOURCE} ]] ; then
        for ipa in `cat ${WHIESOURCE}`
	do
    	    iptables -t filter -A ${CHAIN_MAIN}TMP -d ${ipa} -j RETURN
	done
    fi

    if [[ -f ${DIRSOURCE}${IPSOURCE} ]] ; then
        for ipa in `cat ${IPSOURCE}`
        do
            iptables -t filter -A ${CHAIN_IP}TMP -d ${ipa} -j REJECT
        done
    fi

    if [[ -f ${DIRSOURCE}${URLSOURCE} ]] ; then
        for uri in `cat ${URLSOURCE}`
        do
	    echo -n ""
            # iptables -t filter -A ${CHAIN_URL}TMP -p tcp --dport 80 -m string --string "${uri}" --algo "bm" -j DROP
        done
    fi

    for to in ${TARGET}
    do
	echo "* rule remove target: ${CHAIN_MAIN} -> ${to}"
        iptables -t filter -D ${to} -i ${REVIZOR_IFACE} -s ${REVIZOR_NET} -j ${CHAIN_MAIN}
	while [ $? -eq 0 ]
	do
	    iptables -t filter -D ${to} -i ${REVIZOR_IFACE} -s ${REVIZOR_NET} -j ${CHAIN_MAIN} 2>/dev/null
	done
    done

    for rul in ${CHARULERASE}
    do
        for cha in ${CHAIN_MAIN} ${CHAIN_IP} ${CHAIN_URL}
        do
	    echo "* rule: ${rul} -> ${cha}"
            iptables -t filter ${rul} ${cha}
        done
    done

    for cha in ${CHAIN_IP} ${CHAIN_URL} ${CHAIN_MAIN}
    do
	echo "* rule rename: ${cha}TMP -> ${cha}"
        iptables -t filter -E ${cha}TMP ${cha}
    done

    iptables -t filter -A ${CHAIN_MAIN} -s ${REVIZOR_GW} -j RETURN
    iptables -t filter -A ${CHAIN_MAIN} -s ${REVIZOR_NET} -j ${CHAIN_IP}
    iptables -t filter -A ${CHAIN_MAIN} -s ${REVIZOR_NET} -j ${CHAIN_URL}

    for to in ${TARGET}
    do
	echo "* rule add target: ${CHAIN_MAIN} -> ${to}" 
        iptables -t filter -I ${to} -i ${REVIZOR_IFACE} -s ${REVIZOR_NET} -j ${CHAIN_MAIN}
    done

    for src in ${IPSOURCE} ${URLSOURCE} ${WHIESOURCE}
    do
        if [[ -f ${DIRSOURCE}${src} ]] ; then
            mv -f ${DIRSOURCE}${src} ${DIRBACKUP}
        fi
    done

    rm -f ${DIRSOURCE}${FLAGBUSY}
    rm -f ${DIRSOURCE}${FLAGLOAD}
