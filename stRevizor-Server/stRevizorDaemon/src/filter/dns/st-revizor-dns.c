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

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#include "../../st-revizor-watch.h"
#include "../../st-revizor-inline.h"
#include "../st-revizor-net.h"
#include "st-revizor-dns.h"

static int parse_name(byte_range_t * buf, byte_range_t * name)
{
    unsigned char * name_org;
    unsigned int  len;
    name_org = name->ptr;

    while(1)
    {
        if (buf->ptr == buf->end) { return -1; }
        len = *buf->ptr++;
        if (len == 0) { break; }
        else if ((len & 0xC0) == 0xC0)
        {
            if (buf->ptr + 1 > buf->end) { return -1; }
            buf->ptr++;
            break;
        }
        if ((buf->ptr + len > buf->end) ||
            (name->ptr + len + 2 > name->end)) { return -1; }
        if (name->ptr > name_org) { *(name->ptr++) = '.'; }
        memcpy(name->ptr, buf->ptr, len);
        name->ptr += len;
        buf->ptr  += len;
    }
    *(name->ptr) = 0;

    string_s data = {(char*)name_org, 0};
    string_case(&data, 0);

    name->end = name->ptr;
    name->ptr = name_org;

    return 0;
}

static int parse_question(byte_range_t * buf, dns_question_t * q)
{
    byte_range_t name;
    memset(q, 0, sizeof(*q));
    name.ptr = (unsigned char*)q->name;
    name.end = (unsigned char*)q->name + sizeof(q->name);

    if ((parse_name(buf, &name) < 0) ||
        (buf->ptr + 4 > buf->end)) { return -1; }

    q->type   = htons( *(uint16_t*)buf->ptr );
    q->class_ = htons( *(uint16_t*)(buf->ptr+2) );
    buf->ptr += 4;
    return 0;
}

static int parse_rr(byte_range_t * buf, dns_rr_t * rr)
{
    byte_range_t name;
    memset(rr, 0, sizeof(*rr));
    name.ptr = (unsigned char*)rr->name;
    name.end = (unsigned char*)rr->name + sizeof(rr->name);

    if ((parse_name(buf, &name) < 0) ||
        (buf->ptr + 10 > buf->end)) { return -1; }

    rr->type   = htons( *(uint16_t*)buf->ptr );
    rr->class_ = htons( *(uint16_t*)(buf->ptr+2) );
    rr->ttl    = htonl( *(uint32_t*)(buf->ptr+4) );
    rr->len    = htons( *(uint16_t*)(buf->ptr+8) );
    rr->data   = (char*)buf->ptr + 10;

    if (buf->ptr + 10 + rr->len > buf->end) { return -1; }
    buf->ptr += 10 + rr->len;
    return 0;
}
FUNEXPORT int dns_get_question(const dns_header_udp_t *hdr, size_t len, size_t q_index, dns_question_t * q)
{
    byte_range_t buf;
    size_t q_count;
    q_count = htons(hdr->qcount);

    if (q_index >= q_count || len < sizeof(*hdr)) { return -1; }
    buf.ptr = (uint8_t *)hdr;
    buf.end = buf.ptr + len;
    buf.ptr += sizeof(*hdr);
    do
    {
        if (parse_question(&buf, q) < 0) { return -1; }
    } while (q_index--);
    return 0;
}
FUNEXPORT int dns_get_answer(const dns_header_udp_t * hdr, size_t len, size_t aidx, dns_rr_t * a)
{
    byte_range_t buf;
    dns_question_t qq;
    size_t i, q_count, a_count;;
    q_count = htons(hdr->qcount);
    a_count = htons(hdr->acount);

    if (aidx >= a_count || len < sizeof(*hdr)) { return -1; }

    buf.ptr = (uint8_t *)hdr;
    buf.end = buf.ptr + len;
    buf.ptr += sizeof(*hdr);

    for (i = 0; i < q_count; i++)
    {
        if (parse_question(&buf, &qq) < 0) { return -1; }
        do
        {
            if (parse_rr(&buf, a) < 0) { return -1; }
        }
        while (aidx--);
    }
    return 0;
}

#if defined(HAVE_DNS_DEBUGP) && (HAVE_DNS_DEBUGP == 1)
FUNEXPORT dns_header_udp_t * dns_request_convert(const dns_header_tcp_t *hdr, size_t len)
{
    len = (len - sizeof(unsigned short));
    unsigned char *uhdr = (unsigned char*)hdr;
    return (dns_header_udp_t*)(uhdr + sizeof(unsigned short));
}
FUNEXPORT void dns_dump_tcpreq(const dns_header_tcp_t *hdr, size_t len)
{
    len = (len - sizeof(unsigned short));
    unsigned char *uhdr = (unsigned char*)hdr;
    dns_dump_udpreq((dns_header_udp_t*)(uhdr + sizeof(unsigned short)), len);
}
FUNEXPORT void dns_dump_udpreq(const dns_header_udp_t *hdr, size_t len)
{
    size_t i, q_count, a_count;
    q_count = htons(hdr->qcount);
    a_count = htons(hdr->acount);

    printf("\n");
    for (i = 0; i < len; i++)
    {
        printf("%02x ", 0xff & *(i + (char*)hdr));
        if ((i & 0xf) == 0xf) { printf("\n"); }
    }
    printf("\ndata lenght %d\n", len);
    printf("ID        0x%04hx (%d)(%d)\n", htons(hdr->id), htons(hdr->id), hdr->id);
    printf("flags     0x%04hx (%d)(%d)\n", htons(hdr->flags), htons(hdr->flags), hdr->flags);
    printf("q  count  %u\n",  q_count);
    printf("a  count  %u\n",  a_count);
    printf("ns count  %hu\n", htons(hdr->nscount));
    printf("ar count  %hu\n", htons(hdr->arcount));

    for (i = 0; i < q_count; i++)
    {
        dns_question_t q;
        if (dns_get_question(hdr, len, i, &q) < 0)
        {
            printf("Failed to parse Q[%u]\n", i);
            return;
        }
        printf("\n   Q%u\n", i+1);
        printf("   Name   [%s]\n", q.name);
        printf("   Type   0x%04x\n", q.type);
        printf("   Class  0x%04x\n", q.class_);
    }
    for (i=0; i < a_count; i++)
    {
        dns_rr_t a;
        if (dns_get_answer(hdr, len, i, &a) < 0)
        {
            printf("Failed to parse A[%u]\n", i);
            return;
        }
        printf("\n   A%u\n", i+1);
        printf("   Name   [%s]\n", a.name);
        printf("   Type   0x%04x\n", a.type);
        printf("   Class  0x%04x\n", a.class_);
        printf("   TTL    %u\n", a.ttl);
        printf("   Bytes  %hu\n", a.len);
    }
}
#endif

