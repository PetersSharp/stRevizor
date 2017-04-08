
//DNS Query Program on Linux
//Author : Silver Moon (m00n.silv3r@gmail.com)

#include "st-revizor-dns-test-main.h"

static void dns_changefmt(unsigned char *dns, string_s *host)
{
    int i, lock = 0;
    host->str[host->sz] = '.';
    host->str[(host->sz + 1)] = '\0';

    for(i = 0 ; i < (host->sz + 1); i++)
    {
        if(host->str[i]=='.')
        {
            *dns++ = (i - lock);
            for(; lock < i; lock++)
            {
                *dns++ = (unsigned char)host->str[lock];
            }
            lock++;
        }
    }
    *dns++ = '\0';
    host->str[host->sz] = '\0';
}

FUNEXPORT int dns_gethostbyname(string_s *host, int qtype, int *err)
{
    unsigned char buf[65536],*qname;
    int i, s;
    struct sockaddr_in dest;
    struct DNS_HEADER_t *dns = NULL;
    struct DNS_QUESTION_t *qinfo = NULL;
    struct timeval tmv = { 3, 0 };

    if ((s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)) < 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: create socket failed [%s] -> %s", host->str, strerror(errno));
        *err = errno; return -1;
    }
    __cleanup_set_close(int, sfree, s);

#   if defined(SO_SNDTIMEO)
    if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tmv, sizeof(struct timeval)) != 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: set flag SO_SNDTIMEO [%s] -> %s", host->str, strerror(errno));
        __cleanup_notsupport_p(auto_closeint, sfree);
        *err = errno; return -1;
    }
#   endif
#   if defined(SO_RCVTIMEO)
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tmv, sizeof(struct timeval)) != 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: set flag SO_RCVTIMEO [%s] -> %s", host->str, strerror(errno));
        __cleanup_notsupport_p(auto_closeint, sfree);
        *err = errno; return -1;
    }
#   endif

    dest.sin_family = AF_INET;
    dest.sin_port = htons(HAVE_DNS_CLPORT);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");

    dns = (struct DNS_HEADER_t *)&buf;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER_t)];
    dns_changefmt(qname , host);

    qinfo =(struct DNS_QUESTION_t*)&buf[sizeof(struct DNS_HEADER_t) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons(qtype);
    qinfo->qclass = htons(1);

    if(sendto(s,(char*)buf,sizeof(struct DNS_HEADER_t) + (strlen((const char*)qname)+1) + sizeof(struct DNS_QUESTION_t),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: sendto [%s] -> %s", host->str, strerror(errno));
        __cleanup_notsupport_p(auto_closeint, sfree);
        *err = errno; return -1;
    }
    i = sizeof(dest);
    if(recvfrom(s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        __MACRO_TOSCREEN__(STR_DNS_CHECK "Error: recvfrom [%s] -> %s", host->str, strerror(errno));
        __cleanup_notsupport_p(auto_closeint, sfree);
        *err = errno; return -1;
    }
    __cleanup_notsupport_p(auto_closeint, sfree);
    dns = (struct DNS_HEADER_t*) buf;

    if (ntohs(dns->ans_count) == 0)
    {
        return 0;
    }

    return 1;
}
