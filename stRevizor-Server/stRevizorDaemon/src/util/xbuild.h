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

#ifndef REVIZOR_BUILD_H
#define REVIZOR_BUILD_H

#if defined(HAVE_BITS_WORDSIZE_H)
#   include <bits/wordsize.h>
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#   if defined(__X_BUILD_GCC_VERSION)
#      undef __X_BUILD_GCC_VERSION
#   endif
#   define __X_BUILD_GCC_VERSION ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#endif

#if defined(__i386__) || defined(__i686__) || defined(_M_IX86) || defined(_X86_) || defined(__I86__)
#   if !defined(__X_BUILD_ASM86)
#      define __X_BUILD_ASM86 1
#   endif

#   if defined(__WORDSIZE)

#      if (__WORDSIZE == 64)
#         if !defined(__X_BUILD_64)
#            undef __X_BUILD_64
#         endif
#         define __X_BUILD_64 1
#      elif (__WORDSIZE == 32)
#         if defined(__X_BUILD_32)
#            undef __X_BUILD_32
#         endif
#         define __X_BUILD_32 1
#      endif

#   elif defined(SIZEOF_LONG)

#      if (SIZEOF_LONG == 8)
#         if defined(__X_BUILD_64)
#            undef __X_BUILD_64
#         endif
#         define __X_BUILD_64 1
#      elif (SIZEOF_LONG == 4)
#         if defined(__X_BUILD_32)
#            undef __X_BUILD_32
#         endif
#         define __X_BUILD_32 1
#      endif

#   elif defined(SIZEOF_LONG_LONG)

#      if (SIZEOF_LONG == 16)
#         if defined(__X_BUILD_64)
#            undef __X_BUILD_64
#         endif
#         define __X_BUILD_64 1
#      elif (SIZEOF_LONG_LONG == 8)
#         if defined(__X_BUILD_32)
#            undef __X_BUILD_32
#         endif
#         define __X_BUILD_32 1
#      endif

#   else

#      if !defined(__X_BUILD_64) && !defined(__X_BUILD_32)
#         define __X_BUILD_32 1
#      endif

#   endif

#   if !defined(__WORDSIZE)
#         if defined(__X_BUILD_64)
#            define __WORDSIZE 64
#         elif defined(__X_BUILD_32)
#            define __WORDSIZE 32
#         endif
#   endif

#   if !defined(__WORDSIZE_COMPAT32)
#      define __WORDSIZE_COMPAT32 1
#   endif

#elif defined(__x86_64__)

#   define __ASM_X86_INLINE_SUPPORT
#   define __WORDSIZE 64
#   define __X_BUILD_64

#endif

#if defined(__STDC__)
#    define IS_C89

#    if defined(__STDC_VERSION__)
#        define IS_C90

#        if (__STDC_VERSION__ >= 199409L)
#            define IS_C94
#        endif

#        if (__STDC_VERSION__ >= 199901L)
#            define IS_C99
#        endif
#    endif
#endif

#endif
