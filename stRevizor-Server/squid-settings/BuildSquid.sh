#!/bin/bash

ulimit -n 32768

# if custom gcc binary
#export CC="/usr/bin/gcc4"
#export CXX="/usr/bin/g++4"

make clean

./configure \
  --prefix=/usr/local/squid \
  --exec-prefix=/usr/local/squid \
  --libexecdir=/usr/local/squid/libexec/ \
  --sysconfdir=/usr/local/squid/etc \
  --localstatedir=/usr/local/squid/tmp \
  --libdir=/usr/local/squid/lib \
  --bindir=/usr/local/squid/bin \
  --sbindir=/usr/local/squid/sbin \
  --with-pthreads \
  --without-mit-krb5 \
  --without-heimdal-krb5 \
  --without-nettle \
  --enable-unlinkd \
  --disable-icmp \
  --disable-delay-pools \
  --disable-wccp \
  --disable-wccpv2 \
  --enable-kill-parent-hack \
  --enable-arp-acl \
  --disable-htcp \
  --enable-ssl --enable-ssl-crtd --with-openssl \
  --enable-default-err-language=UTF-8 \
  --enable-linux-netfilter \
  --disable-ident-lookups \
  --enable-follow-x-forwarded-for \
  --with-maxfd=32768 \
  --enable-storeio=diskd,ufs

#  Transparent port options:
#  --enable-linux-netfilter  (Linux Netfilter)
#  --enable-ipfw-transparent (FreeBSD IPFW-style firewalling)
#  --enable-ipf-transparent  (IPFilter-style firewalling)
#  --enable-pf-transparent   (PF network address redirection)

make

# /etc/rc.d/init.d/squid stop
# make install
# make clean
