
#ifndef REVIZOR_URL_LANG_H
#define REVIZOR_URL_LANG_H

#define STR_SSL_TEXT "\t\t<SSL context ..>"
#define STR_URL_BLOCKR " memory URL blocking response -> %s"
#define STR_URL_SRV "Server"
#define STR_URL_CLI "Client"

#if defined(__URL_ERR_ENGINE)
static const char *errstrengine[] = {
    "client read request error, skip..",
    "client send response error, skip..",
    "accept error",
    "socket get destination",
    "client socket not support set timeout",
    "read client request error",
    "client HTTP request header empty",
    "client HTTP request method not implemented",
    "client HTTP request header and destination error",
    "remote proxy socket error",
    "remote socket not support set timeout",
    "remote proxy connect error",
    "remote proxy send CONNECT error",
    "remote proxy timeout, close..",
    "remote proxy receive response error",
    "proxy response error parse, close..",
    "tcpd reject this client",
    "not send clent data to proxy",
    "creaate proxy request",
    "remote proxy set nonblock error",
    "destination is local ip, loop detected"
};
#undef __URL_ERR_ENGINE
#endif

#if defined(__URL_ERR_TCP_ENGINE)
#   if (defined(DEBUG_URL_PRINT) && defined(DEBUG_URL_SENDRCV))
static const char *statusstrings[] = {
    "Receive socket negative",
    "Receive operation end",
    "Receive socket shutdown",
    "Receive socket timeout",
    "Receive socket successful",
    "Send socket negative",
    "Send socket end",
    "Send socket shutdown",
    "Send socket timeout",
    "Send socket successful",
    "Continue ..",
    "No errors (0 return)"
};
#   endif
#undef __URL_ERR_TCP_ENGINE
#endif

#if defined(__URL_ERR_LOOP)
#endif

#endif
