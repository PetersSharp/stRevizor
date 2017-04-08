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

#pragma pack(push, 1)
#pragma pack(pop)

#define DNS_GET_(hdr, A, B) \
    ((htons((hdr)->flags) >> A) & B)

#define DNS_SET_(hdr, N, A) \
    ((hdr)->flags |= ((N) << A))

#define DNS_CLR_(hdr, N, A) \
    ((hdr)->flags &= ~((N) << A))

#define DNS_GET_QR(hdr)     DNS_GET_(hdr, 15, 0x01)
#define DNS_GET_OPCODE(hdr) DNS_GET_(hdr, 11, 0x0F)
#define DNS_GET_RCODE(hdr)  DNS_GET_(hdr, 0,  0x0F)

#define DNS_SET_QR(hdr,n)  { \
        unsigned char dr = DNS_GET_(hdr, 15, 0x01); \
        DNS_CLR_(hdr, dr, 7); \
        DNS_SET_(hdr, n, 7); \
    }

#define DNS_SET_OPCODE(hdr,n)  { \
        DNS_CLR_(hdr, ((n > 0) ? 0 : 1), 3); \
        DNS_SET_(hdr, n, 3); \
    }

#define DNS_SET_RCODE(hdr,n)  { \
        unsigned char dr = DNS_GET_(hdr, 0, 0x0F); \
        DNS_CLR_(hdr, dr, 8); \
        DNS_SET_(hdr, n, 8); \
    }

typedef struct __dns_header_tcp__
{
    unsigned short _pad0;
    uint16_t id;
    uint16_t flags;
    uint16_t qcount;
    uint16_t acount;
    uint16_t nscount;
    uint16_t arcount;
} dns_header_tcp_t;

typedef struct __dns_header_udp__
{
    uint16_t  id;
    uint16_t  flags;
    uint16_t  qcount;
    uint16_t  acount;
    uint16_t  nscount;
    uint16_t  arcount;
} dns_header_udp_t;

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
    dns_header_udp_t udphdr;
    dns_header_tcp_t tcphdr;
} dns_io_buf_u;

typedef struct __byte_range__
{
    uint8_t * ptr;
    uint8_t * end;
} byte_range_t;

typedef struct __srv_socket_dns__
{
    sa_proxy_t      *sa1;
    sa_proxy_t      *sa2;
    st_table_lock_t *reqlock;
    void           **obj;
    size_t           reqsize;
    uint64_t         reqnext;
    pending_req_t    requests[HAVE_NET_QUEUE];
} srv_socket_dns_t;

    /*
    // CallBack
    // int  dns_relay_q(srv_socket_dns_t*, void*);
    // int  dns_relay_r(srv_socket_dns_t*, void*);
    // int  dns_relay_tcp(srv_socket_dns_t*, void*);
    */

int  dns_init_table(void);
int  dns_filter(void);
int  dns_get_question(const dns_header_udp_t*, size_t, size_t, dns_question_t *);
int  dns_get_answer(const dns_header_udp_t*, size_t, size_t, dns_rr_t*);
void dns_dump_udpreq(const dns_header_udp_t*, size_t);
void dns_dump_tcpreq(const dns_header_tcp_t*, size_t);

dns_header_udp_t * dns_request_convert(const dns_header_tcp_t*, size_t);

int  dns_event_poll(srv_transport_t*);

#endif

