
#ifndef REVIZOR_DNS_TEST_REQUEST_H
#define REVIZOR_DNS_TEST_REQUEST_H

#define T_A 1      // ipv4 address
#define T_NS 2     // nameserver
#define T_CNAME 5  // canonical name
#define T_SOA 6    // start of authority zone
#define T_PTR 12   // domain name pointer
#define T_MX 15    // mail server
 
FUNEXPORT int dns_gethostbyname(string_s * , int, int*);

#pragma GCC diagnostic ignored "-pedantic"
struct __attribute__((packed)) DNS_HEADER_t
{
    unsigned short id; // identification number
 
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag
 
    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available
 
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};
#pragma GCC diagnostic pop

struct DNS_QUESTION_t
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct DNS_RDATA_t
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct DNS_RECORD_t
{
    unsigned char *name;
    struct DNS_RDATA_t *resource;
    unsigned char *rdata;
};

#endif
