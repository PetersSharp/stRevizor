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

#ifndef REVIZOR_FILE_H
#define REVIZOR_FILE_H

#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif

#if !defined(HAVE_CONFIG_H_LOAD) && defined(HAVE_CONFIG_H)
#   define HAVE_CONFIG_H_LOAD 1
#   include "../config.h"
#endif

void file_clears(string_s*, const string_s*, int);
int  file_exists(string_s*, string_s*);
int  file_exists_fp(string_s*);
long file_size(FILE*);
void file_delete(string_s*, string_s*);
void file_delete_fp(string_s*);
void file_backup(string_s*, string_s*);
void file_backup_fp(string_s*, const string_s*, int);
void file_create(string_s*, const string_s*, string_s*);
void file_create_fp(string_s*, string_s*);
void file_read(string_s*, fwall_cb);
void file_move(string_s*, string_s*, string_s*);
void file_move_fp(string_s*, string_s*);

#endif
