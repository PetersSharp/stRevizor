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

static void ipset_start(void)
{
    __MACRO_CREATEBIN__();
    __BUFFER_CREATE__(fwall);

    __MACRO_SETBIN__(ENUMBIN_IPT);
    for (int n = 0; n < __NELE(ipt_chain_name); n++)
    {
        for (int i = 0; i < __NELE(ipt_chain_flags3); i++)
        {
            /* create tmp chains */
            __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINTMP, ipt_chain_flags3[i], ipt_chain_name[n]);
        }
    }
    __MACRO_SETBIN__(ENUMBIN_IPS);
    for (int n = 0; n < __NELE(ips_table_name); n++)
    {
        __MACRO_IPTRULE__(bpath, fwall, , IPS_FLUSH,  ips_table_name[n], "");
        __MACRO_IPTRULE__(bpath, fwall, , IPS_DELETE, ips_table_name[n], "");
        __MACRO_IPTRULE__(bpath, fwall, , IPS_CREATE, ips_table_name[n], "");
    }
}
static void ipset_end(void)
{
    __MACRO_CREATEBIN__();
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __BUFFER_CREATE__(fwall);

    /* add first rule, pass gw ip */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_IPLIST, "I", IPT_CHAINMAIN, HAVE_IPT_GW, IPT_RULE_RETURN);

    /* add rule to white list and blacklist (ipset (IPT_CHAINWHT)) */
    __MACRO_IPTRULE__(bpath, fwall, , IPS_WHTLIST, "I", IPT_CHAINMAIN, ips_table_name[0]);

    /* add rule to white list and blacklist (ipset (IPT_CHAINIP)) */
    __MACRO_IPTRULE__(bpath, fwall, , IPS_IPPLIST, "A", IPT_CHAINMAIN, ips_table_name[1], HAVE_IPT_FINAL);

    /* erase rule in target FORWARD */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_TOFORWARD, "D", IPT_CHAINFWD, HAVE_IPT_IFACE, HAVE_IPT_NET, IPT_CHAINMAIN);

    for (int n = 0; n < __NELE(ipt_chain_name); n++)
    {
        for (int i = 0; i < __NELE(ipt_chain_flags2); i++)
        {
            /* erase and delete main chains */
            __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAIN, ipt_chain_flags2[i], ipt_chain_name[n]);
        }
        /* rename chains */
        __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINRENAME, ipt_chain_name[n], ipt_chain_name[n]);
    }

    /* add rule in target FORWARD */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_TOFORWARD, "I", IPT_CHAINFWD, HAVE_IPT_IFACE, HAVE_IPT_NET, IPT_CHAINMAIN);

    /* flush final chains */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAIN, "F", IPT_CHAINEND);

    /* add all rule to final chains */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINEND, HAVE_IPT_FINAL);

    /* add finalize RETURN rule to IPT_CHAINMAIN and IPT_CHAINURL */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINURL,  IPT_RULE_RETURN);
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINMAIN, IPT_RULE_RETURN);
}
static void ipset_clear(void)
{
    __MACRO_CREATEBIN__();
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __BUFFER_CREATE__(fwall);

    for (int n = 0; n < __NELE(ipt_chain_name); n++)
    {
        for (int i = 0; i < __NELE(ipt_chain_flags2); i++)
        {
            /* delete tmp chains */
            __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINTMP, ipt_chain_flags2[i], ipt_chain_name[n]);
        }
    }
}
static void * ipset_biplist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPS);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_ADD, IPT_CHAINIP, "", data.str);
    return NULL;
}
static void * ipset_urllist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_URLLIST, "A", IPT_CHAINURL, data.str, IPT_CHAINEND);
    return NULL;
}
static void * ipset_wiplist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPS);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_ADD, IPT_CHAINWHT, "", data.str);
    return NULL;
}
static void * ipset_portlist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);

    __MACRO_SETBIN__(ENUMBIN_IPS);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_CREATE, IPT_CHAINIPP, data.str);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_FLUSH, IPT_CHAINIPP, data.str);

    /* wait ipset timeout to write/set data to kernel */
    sleep(1);
    __MACRO_TOSCREEN__("[Net Filter]: add ipset table: %s%s -> %s", IPT_CHAINIPP, data.str, IPT_CHAINMAIN);

    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_FILTERIPP, "A", IPT_CHAINMAIN, data.str, IPT_CHAINIPP, data.str, HAVE_IPT_FINAL);
    return NULL;
}
static void * ipset_portlist_clear(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_TOSCREEN__("[Net Filter]: delete ipset table: %s%s", IPT_CHAINIPP, data.str);

    __MACRO_SETBIN__(ENUMBIN_IPS);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_FLUSH, IPT_CHAINIPP, data.str);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_DELETE, IPT_CHAINIPP, data.str);
    return NULL;
}
static void * ipset_piplist(nfbin_t bpath, string_s data)
{
    char *src;
    if ((src = strdup(data.str)) == NULL)
    {
        return NULL;
    }
    fwall_pip_t pip = string_split(src);
    if ((pip.ip.str != NULL) || (pip.port.str != NULL) || (pip.cnt == 2))
    {
        __BUFFER_CREATE__(fwall);
        __MACRO_SETBIN__(ENUMBIN_IPS);
        __MACRO_IPTRULE__(bpath, fwall, NULL, IPS_ADD, IPT_CHAINIPP, pip.port.str, pip.ip.str);
    }
    if (src != NULL) { free(src); }
    if (pip.port.str != NULL) { free(pip.port.str); }
    if (pip.ip.str != NULL) { free(pip.ip.str); }
    return NULL;
}
static void ipset_old_portlist_clear(nfbin_t bpath, int idx)
{
    if ((idx < 0) || (idx >= __NELE(&insource)))
    {
        return;
    }
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", HAVE_WATCH_BACKUP, insource[idx].str);

    if (access(fpath.str, F_OK) != -1)
    {
        file_read(fpath, bpath, ipset_portlist_clear);
    }
}
