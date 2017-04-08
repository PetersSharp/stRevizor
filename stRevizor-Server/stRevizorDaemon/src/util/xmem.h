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

#ifndef REVIZOR_XMEM_H
#define REVIZOR_XMEM_H

#   if (defined(__GNUC__) && ((__GNUC__ >= 3) && (__GNUC_MINOR__ >= 3)))
#      define __cleanup_new_free(A)       __FUNCLEANUP(A,free)   A *
#      define __cleanup_set_free(A,B,C)   __FUNCLEANUP(A,free)   A * B = (A *) C
#      define __cleanup_new_close(A)      __FUNCLEANUP(A,close)  A *
#      define __cleanup_set_close(A,B,C)  __FUNCLEANUP(A,close)  A * B = (A *)& C
#      define __cleanup_new_fclose(A)     __FUNCLEANUP(A,fclose) A *
#      define __cleanup_set_fclose(A,B,C) __FUNCLEANUP(A,fclose) A * B = (A *) C
#      define __cleanup_set_unlock(A,B,C) __FUNCLEANUP(A,unlock) A * B = (A *)& C
#      define __FUNCLEANUP(A,B) __attribute__((cleanup(__EV(auto_,__EV(B,A)))))
#      define __cleanup_notsupport(...)
#      define __cleanup_notsupport_p(...)
#   else
#      define __cleanup_new_free(A)       A *
#      define __cleanup_set_free(A,B,C)   A * B = (A *) C
#      define __cleanup_new_close(A)      A *
#      define __cleanup_set_close(A,B,C)  A * B = (A *)& C
#      define __cleanup_new_fclose(A)     A *
#      define __cleanup_set_fclose(A,B,C) A * B = (A *) C
#      define __cleanup_set_unlock(A,B,C) A * B = (A *)& C
#      define __cleanup_notsupport(A,B) { A(B); B = NULL; }
#      define __cleanup_notsupport_p(A,B) { A(&B); B = NULL; }
#   endif

#endif
