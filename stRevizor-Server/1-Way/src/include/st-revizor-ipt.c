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

static void iptables_start(void)
{
    __MACRO_CREATEBIN__();
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __BUFFER_CREATE__(fwall);

    for (int n = 0; n < __NELE(ipt_chain_name); n++)
    {
        for (int i = 0; i < __NELE(ipt_chain_flags3); i++)
        {
            /* create tmp chains */
            __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINTMP, ipt_chain_flags3[i], ipt_chain_name[n]);
        }
    }
    /* create final chains */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAIN, "N", IPT_CHAINEND);
}
static void iptables_end(void)
{
    __MACRO_CREATEBIN__();
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __BUFFER_CREATE__(fwall);

    /* add first rule, pass gw ip */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_IPLIST, "I", IPT_CHAINMAIN, HAVE_IPT_GW, IPT_RULE_RETURN);

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

    /* add sub chains rules (IPT_CHAINIP, IPT_CHAINURL) */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_FILTERIP, "A", IPT_CHAINMAIN, IPT_CHAINIP);
    __MACRO_IPTRULE__(bpath, fwall, , IPT_FILTERURL, "A", IPT_CHAINMAIN, IPT_CHAINURL);

    /* add rule in target FORWARD */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_TOFORWARD, "I", IPT_CHAINFWD, HAVE_IPT_IFACE, HAVE_IPT_NET, IPT_CHAINMAIN);

    /* flush final chains */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAIN, "F", IPT_CHAINEND);

    /* add all rule to final chains */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINEND, HAVE_IPT_FINAL);

    /* add finalize RETURN rule to IPT_CHAINIP and IPT_CHAINURL and IPT_CHAINMAIN */
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINIP,   IPT_RULE_RETURN);
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINURL,  IPT_RULE_RETURN);
    __MACRO_IPTRULE__(bpath, fwall, , IPT_CHAINFINAL, IPT_CHAINMAIN, IPT_RULE_RETURN);
}
static void iptables_clear(void)
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
static void * iptables_portlist_clear(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_TOSCREEN__("\tDelete iptables chains: %s%s -> %s", IPT_CHAINIPP, data.str, IPT_CHAINMAIN);

    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_CHAINPIP, "F", IPT_CHAINIPP, data.str);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_CHAINPIP, "X", IPT_CHAINIPP, data.str);
    return NULL;
}
static void * iptables_biplist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_IPLIST, "A", IPT_CHAINIP, data.str, IPT_CHAINEND);
    return NULL;
}
static void * iptables_urllist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_URLLIST, "A", IPT_CHAINURL, data.str, IPT_CHAINEND);
    return NULL;
}
static void * iptables_wiplist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_IPLIST, "A", IPT_CHAINMAIN, data.str, IPT_RULE_RETURN);
    return NULL;
}
static void * iptables_portlist(nfbin_t bpath, string_s data)
{
    __BUFFER_CREATE__(fwall);
    __MACRO_TOSCREEN__("\tAdd iptables chains: %s%s -> %s", IPT_CHAINIPP, data.str, IPT_CHAINMAIN);

    __MACRO_SETBIN__(ENUMBIN_IPT);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_CHAINPIP, "N", IPT_CHAINIPP, data.str);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_CHAINPIP, "F", IPT_CHAINIPP, data.str);
    __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_FILTERIPP, "A", IPT_CHAINMAIN, data.str, data.str);
    return NULL;
}
static void * iptables_piplist(nfbin_t bpath, string_s data)
{
    char *src;
    if ((src = strdup(data.str)) == NULL)
    {
        return NULL;
    }
    fwall_pip_t pip = strsplit(src);
    if ((pip.ip.str != NULL) || (pip.port.str != NULL) || (pip.cnt == 2))
    {
        __MACRO_SETBIN__(ENUMBIN_IPT);
        __BUFFER_CREATE__(fwall);
        __MACRO_IPTRULE__(bpath, fwall, NULL, IPT_IPPLIST, "A", pip.port.str, pip.ip.str);
    }
    if (src != NULL) { free(src); }
    if (pip.port.str != NULL) { free(pip.port.str); }
    if (pip.ip.str != NULL) { free(pip.ip.str); }
    return NULL;
}
static void iptables_old_portlist_clear(nfbin_t bpath, int idx)
{
    if ((idx < 0) || (idx >= __NELE(&insource)))
    {
        return;
    }
    __BUFFER_CREATE__(fpath);
    __MACRO_SNPRINTF__(fpath, , "%s/%s", HAVE_WATCH_BACKUP, insource[idx].str);

    if (access(fpath.str, F_OK) != -1)
    {
        file_read(fpath, bpath, iptables_portlist_clear);
    }
}
