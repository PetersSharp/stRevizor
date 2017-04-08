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

#ifndef REVIZOR_WATCH_DEBUG_H
#define REVIZOR_WATCH_DEBUG_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../config.h"
#endif

#   if defined(DEBUG_TO_SYSLOG)
#      undef DEBUG_TO_SYSLOG
#   endif

#   if (defined(HAVE_DEBUG_SYSLOG) && (HAVE_DEBUG_SYSLOG == 1))
#      define DEBUG_TO_SYSLOG 1
#   endif

#   if defined(DEBUG_TO_SYSLOG)
#       undef  HAVE_WATCH_SYSLOG
#       define HAVE_WATCH_SYSLOG 1
#   endif

#   if defined(DEBUG_NODAEMON)
#      undef DEBUG_NODAEMON
#   endif

#   if (!defined(HAVE_WATCH_DAEMON) || (HAVE_WATCH_DAEMON == 0))
#      define DEBUG_NODAEMON 1
#   endif

#   if defined(DEBUG_PRINT)
#      undef DEBUG_PRINT
#   endif

#   if (defined(DEBUG_NODAEMON) || defined(DEBUG_TO_SYSLOG))
#      define DEBUG_PRINT 1
#   endif

#   if defined(DEBUG_SYSLOG)
#      undef DEBUG_SYSLOG
#   endif

#   if ((defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1)) || defined(DEBUG_NODAEMON))
#      define DEBUG_SYSLOG 1
#   endif

#   if (defined(HAVE_WATCH_SYSLOG) && (HAVE_WATCH_SYSLOG == 1))
#      if defined(DEBUG_TO_SYSLOG)
#          define __MACRO_TOSYSLOG__(A, ...) syslog(LOG_DEBUG, __VA_ARGS__)
#      else
#          define __MACRO_TOSYSLOG__(A, ...) syslog(A, __VA_ARGS__)
#      endif
#   else
#      if !defined(DEBUG_NODAEMON)
#          define __MACRO_TOSYSLOG__(A, ...)
#      else
#          define __MACRO_TOSYSLOG__(A, B, ...) printf(B "\n", __VA_ARGS__)
#      endif
#   endif

#   if !defined(DEBUG_NODAEMON)
#      if defined(DEBUG_TO_SYSLOG)
#         define __MACRO_TOSCREEN__(A, ...) syslog(LOG_DEBUG, A, __VA_ARGS__)
#         define __MACRO_IS_TOSCREEN 1
#      else
#         define __MACRO_TOSCREEN__(A, ...)
#      endif
#   else
#      define __MACRO_TOSCREEN__(A, ...) printf(A "\n", __VA_ARGS__)
#      define __MACRO_IS_TOSCREEN 1
#   endif

#   if defined(DEBUG_SYSLOG)
#      define __MACRO_ISSYSLOG__(A) A
#   else
#      define __MACRO_ISSYSLOG__(A)
#   endif

#endif
