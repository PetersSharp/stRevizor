#!/bin/bash

#    tftp 91.202.4.250 -m binary -c put blocklist.fw
#    TEST=$?
#    echo "return ${TEST} $?"

    for i in `ls -1 squid*.fw`
    do
        echo ${i}
        tftp 91.202.4.251 -m binary -c put ${i}
    done
