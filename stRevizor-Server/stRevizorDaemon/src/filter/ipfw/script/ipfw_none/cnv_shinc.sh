#!/bin/bash
# stRevizor firewall wrapper

set -eu


    for i in `dir -1 *.inc`
    do
        FNAME=`echo ${i} | awk -F"." '{ print $1 ".tobin" ; }'`
        echo ${FNAME}
        mv -f ${i} ${FNAME}
    done

rm -f ./*.o
rm -f ./*.inc
