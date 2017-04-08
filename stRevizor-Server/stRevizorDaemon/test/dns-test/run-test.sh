#!/bin/bash

#--------------------------------------------------------------------------------------
#
#    - TEST part
#

EXEC="st-revizor-dns-test-main"

valgrind \
--leak-check=full \
--show-leak-kinds=all \
--show-reachable=no \
--track-origins=yes \
--vgdb=yes \
--error-limit=no \
--log-file="logfile.out" \
-v ./${EXEC}

#echo -e "\n\tLDD link library: ${EXEC}\n"
#ldd .build/revizor-watch/${EXEC}

