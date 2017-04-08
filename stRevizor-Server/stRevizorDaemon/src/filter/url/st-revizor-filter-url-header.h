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

#ifndef REVIZOR_URL_HEADER_H
#define REVIZOR_URL_HEADER_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

typedef enum __http_parse_status_ {
    HTTP_PARSE_OK         =  0,
    HTTP_BAD_RESPONSE     = -1,
    HTTP_HEADER_ERROR     = -2,
    HTTP_HEADER_NOTIMPL   = -3,
    HTTP_HEADER_EMPTY     = -4,
    HTTP_PARSE_SSL        = -5,
    HTTP_PARSE_CHANGE_DST = -6
} http_parse_status_e;

enum __http_field_ {
    HTTP_FIELD_PORT   = 0,
    HTTP_FIELD_HOST   = 1,
    HTTP_FIELD_URL    = 2,
    HTTP_FIELD_ORIGIN = 3,
    HTTP_FIELD_SLIMIT = 4,
    HTTP_FIELD_METHOD = 5
};

enum __http_method_ {
    HTTP_METHOD_GET     = 0,
    HTTP_METHOD_PUT     = 1,
    HTTP_METHOD_POST    = 2,
    HTTP_METHOD_PATCH   = 3,
    HTTP_METHOD_HEAD    = 4,
    HTTP_METHOD_TRACE   = 5,
    HTTP_METHOD_OPTIONS = 6,
    HTTP_METHOD_DELETE  = 7,
    HTTP_METHOD_CONNECT = 8
};

typedef struct __core_http_parse_s_ {
    int    limit;
    string_s field[3];
} http_parse_s;

void                http_init_parse_s(http_parse_s*);
int                 http_check_response(string_s*);
http_parse_status_e http_parse_request(string_s*, http_parse_s*);
http_parse_status_e https_parse_sni(string_s*, http_parse_s*);
http_parse_status_e https_parse_dst(sa_socket_t*, string_s *, http_parse_s *h, int*);

#endif

