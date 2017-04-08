
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

#define __STR_S_STR(A)  (A.str)
#define __STR_S_SZ(A)   (A.sz)

#   define EVALUTE_SENDERR_ARG_9(_1,_2,_3,_4,_5,_6,_7,_8,_9)
#   define EVALUTE_SENDERR_ARG_8(_1,_2,_3,_4,_5,_6,_7,_8)
#   define EVALUTE_SENDERR_ARG_7(_1,_2,_3,_4,_5,_6,_7)
#   define EVALUTE_SENDERR_ARG_6(_1,_2,_3,_4,_5,_6)
#   define EVALUTE_SENDERR_ARG_5(_1,_2,_3,_4,_5)
#   define EVALUTE_SENDERR_ARG_4(_1,_2,_3,_4) tcp_send_error__(_1,_2,_3,_4)
#   define EVALUTE_SENDERR_ARG_3(_1,_2,_3) tcp_send_error__(_1,__STR_S_STR(_2),__STR_S_SZ(_2),_3)
#   define EVALUTE_SENDERR_ARG_2(_1,_2) tcp_send_error__(_1,__STR_S_STR(_2),__STR_S_SZ(_2),NULL)
#   define EVALUTE_SENDERR_ARG_1(_1)
#   define EVALUTE_SENDERR_ARG_0(...)

#define tcp_send_error(...) \
        __EV(EVALUTE_SENDERR_ARG_, __EVALUTE_FUNC_ARGS(__VA_ARGS__))(__VA_ARGS__)

FUNINLINE const char * tcp_send_error__(sa_socket_t *sa, char *errtag, int errsz, const char **errstr)
{
    if (tcp_send(SOCK_CLIENT, sa, errtag, (ssize_t)errsz) < 0)
    {
        return ((errstr != NULL) ?  errstr[1] : (const char*)&"" );
    }
    return NULL;
}
