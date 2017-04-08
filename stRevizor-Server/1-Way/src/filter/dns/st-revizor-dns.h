//--------------------------------------------------------------------------------------
//
//    stRevizor package - Registry RosKomNadzor blacklist converter to iptables/ipset,
//              Win GUI client, *nix server part and more..
//
//    stRevizor server - Registry RosKomNadzor blacklist watch daemon.
//              Watch FTP/TFTP directory for the presence of RosKomNadzor
//              registry update files and makes changes to the iptables/ipset.
//
//              Copyright (C) 2015-2017, @PS
//
//    Everyone is permitted to copy and distribute verbatim copies
//    of this license document, but changing it is not allowed.
//
//    See file LICENSE.md in directory distribution.
//
//    You can find latest source:
//     - https://github.com/PetersSharp/stRevizor
//     - https://github.com/PetersSharp/stRevizor/releases/latest
//     - https://github.com/PetersSharp/stRevizor/pulse
//
//    * $Id$
//    * commit date: $Format:%cd by %aN$
//
//--------------------------------------------------------------------------------------

#ifndef REVIZOR_DNS_H
#define REVIZOR_DNS_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#define STR_DNS_DEBUG "[DNS filter]: ? "

#pragma pack(push, 1)
#pragma pack(pop)

#define DNS_GET_QR(hdr)     ( (htons((hdr)->flags) >> 15) & 0x01 )
#define DNS_GET_OPCODE(hdr) ( (htons((hdr)->flags) >> 11) & 0x0F )
#define DNS_GET_RCODE(hdr)  ( htons((hdr)->flags) & 0x0F )

typedef struct __dns_header__
{
    uint16_t  id;
    uint16_t  flags;
    uint16_t  qcount;
    uint16_t  acount;
    uint16_t  nscount;
    uint16_t  arcount;
} dns_header_t;

typedef struct __dns_question__
{
    char      name[256];
    uint16_t  type;
    uint16_t  class_;
} dns_question_t;

typedef struct __dns_rr__
{
    char      name[256];
    uint16_t  type;
    uint16_t  class_;
    uint32_t  ttl;
    uint16_t  len;
    char    * data;
} dns_rr_t;

typedef union __dns_io_buf__
{
    char         raw[64*1024];
    dns_header_t hdr;
} dns_io_buf_t;

typedef struct __byte_range__
{
    uint8_t * ptr;
    uint8_t * end;
} byte_range_t;

// CallBack
// int  dns_relay_q(srv_socket_t*, void*);
// int  dns_relay_r(srv_socket_t*, void*);

int  dns_init_table(void);
int  dns_filter(void);
int  dns_get_question(const dns_header_t*, size_t, size_t, dns_question_t *);
int  dns_get_answer(const dns_header_t*, size_t, size_t, dns_rr_t*);
void dns_dump_response(const dns_header_t*, size_t);

#endif

