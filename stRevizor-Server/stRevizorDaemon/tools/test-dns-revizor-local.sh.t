#!/bin/bash

    echo -e "\tRevizor DNS filter local test"

    if [[ {{ENABLEDNS}} != 1 ]] ;
    then
        echo -e "\tRevizor compiled without DNS filter, exit.."
        exit
    fi
    if [[ ${1} == "" ]] ;
    then
        echo -e "\tNo host given ..\n\tuse: ${0} test.host.com"
        exit
    fi

    echo -e "\tUDP DNS filter local test:"
    {{IPTBIN}} -t nat -I OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port {{DNSCLPORT}}
    host -v -a ${1} 127.0.0.1
    {{IPTBIN}} -t nat -D OUTPUT -o lo -p udp --dport 53 -j REDIRECT --to-port {{DNSCLPORT}}

    if [[ {{ENABLEDNSTCP}} == 1 ]] ;
    then
        echo -e "\tTCP DNS filter local test:"
        {{IPTBIN}} -t nat -I OUTPUT -o lo -p tcp --dport 53 -j REDIRECT --to-port {{DNSCLPORT}}
        host -v -T -a ${1} 127.0.0.1
        {{IPTBIN}} -t nat -D OUTPUT -o lo -p tcp --dport 53 -j REDIRECT --to-port {{DNSCLPORT}}
    fi

