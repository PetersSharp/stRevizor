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

/* Hash Tree */

static void hash_tree_sort_(hash_tree_ele_t **htr, unsigned long idx)
{
    unsigned long i, low;
    hash_tree_ele_t *val1, *val2, **ele0 = htr;
    for (low = 0; low < (idx - 1); low++)
    {
        for (i = 0; i < (idx - low - 1); i++)
        {
            __builtin_prefetch(ele0[(i+1)]);
            if (ele0[i]->hash > ele0[(i+1)]->hash)
            {
                val1        = ele0[i];
                val2        = ele0[(i+1)];
                ele0[i]     = val2;
                ele0[(i+1)] = val1;
            }
        }
    }
}
static int hash_tree_search_(hash_tree_ele_t **htr, unsigned long idx, unsigned long hash, unsigned long *ret)
{
    *ret = 0;

    if ((htr == NULL) || (idx == 0))
    {
        return -1;
    }
    unsigned long hi = idx, low = 0UL;
    __builtin_prefetch(htr + ((idx + 1) * sizeof(hash_tree_ele_t)), 0,1,1);
    while (low < hi)
    {
        unsigned long mid = ((hi + low) / 2);
        if (!htr[mid])                   {  continue; }
        else if (htr[mid]->hash == hash) { *ret = mid; return 0; }
        else if (htr[mid]->hash > hash)  {  hi  = mid; }
        else                             {  low = (mid + 1); }
    }
    return -1;
}
static hash_tree_ele_t ** hash_tree_alloc_(hash_tree_ele_t **htr, unsigned long *idx, unsigned long *cnt)
{
    hash_tree_ele_t **hto = NULL;

    do {
        if ((htr == NULL) || (*htr == NULL))
        {
            if ((hto = calloc(1, sizeof(hash_tree_ele_t*))) == NULL)        { break; }
            if ((hto[(*idx)] = calloc(1, sizeof(hash_tree_ele_t))) == NULL) { break; }
            hto[(*idx)]->treeidx = 0;
        }
        else
        {
            if ((hto = realloc(htr, (((*idx) + 1) * sizeof(hash_tree_ele_t*)))) == NULL) { break; }
            if ((hto[(*idx)] = calloc(1, sizeof(hash_tree_ele_t))) == NULL) { break; }
        }
        (*idx)++;
        if ((cnt[1] > 0) && ((cnt[1] % 100) == 0))
        {
            __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "URL complette parse -> %lu uri", cnt[1]);
        }
        return hto;

    } while(0);

    if (hto != NULL)
    {
        hash_tree_ele_free_(hto, *(idx));
        free(hto);
    }
    return NULL;
}

/* Hash Tree public */

FUNEXPORT hash_tree_t * hash_tree_url_read(string_s *fpath, void *ppath, hash_portlist_cb plcb)
{
    __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: begin create tree from file -> %s", fpath->str);
    __cleanup_new_fclose(FILE) fp;

    if ((fp = fopen(fpath->str, "r")) == NULL)
    {
        __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: error open file -> %s (%s)", fpath->str, strerror(errno));
        return NULL;
    }

    int i, n;
    char *param[5] = { NULL };
    string_s sline = {0}, data = {0};
    hash_tree_t *htr;

    if ((htr = calloc(1, sizeof(hash_tree_t))) == NULL)
    {
        __MACRO_CLEAR_HASHTREE__(fp, NULL, NULL);
        return NULL;
    }
    __cleanup_set_free(hash_tree_t, htrfree, htr);
    memset(htr->eleidx, 0, (sizeof(unsigned long) * 3));
    errno = 0;

    while (getline(&sline.str, ((size_t*)&sline.sz), fp) != -1)
    {
        if ((sline.sz < 2) || (sline.str == NULL))
        {
            continue;
        }

        param[0] = param[1] = param[2] = param[3] = param[4] = NULL;
        string_trim(&sline);

        if (
            ((n = string_split((char * const)sline.str, (char**)param, __NELE(param))) < 3) ||
            (param[0] == NULL) || (param[0][0] != 'U') || (param[1] == NULL) || (param[2] == NULL)
           )
        {
            continue;
        }

        hash_tree_ele_t *htele = NULL;
        unsigned long idx, hash;

        n = ((n > 4) ? 4 : n);

        for (i = 1; i < n; i++)
        {
            if (param[i] == NULL)
            {
                break;
            }
            __cleanup_new_free(char) urls = NULL;

            data.str = param[i];
            string_trim(&data);
            string_case(&data,0);

             /* correct url path, if exists, add '/' */
            if ((i == HASH_TREE_ELE_URL) && (data.str[0] != '/'))
            {
                char *urls;
                if ((urls = calloc(1, (data.sz + 1))) == NULL)
                {
                    continue;
                }
                urls[0] = '/';
                memcpy(urls + 1, data.str, data.sz);
                data.str = urls; ++data.sz;
            }

            hash = hash_string_(&data);

            switch(i)
            {
                 /* case 0: marker index 'U' */
                case HASH_TREE_ELE_MARK:
                {
                    __MACRO_CLEAR_HASHTREE__(-1, NULL, urls);
                    continue;
                }
                 /* case 1: http/https port - URL filter */
                case HASH_TREE_ELE_PORT:
                {
                    switch (hash_tree_search_(htr->tree, htr->treeidx, hash, &idx))
                    {
                        case -1:
                        {
                            if ((htr->tree = hash_tree_alloc_(htr->tree, &htr->treeidx, htr->eleidx)) == NULL)
                            {
                                __MACRO_CLEAR_HASHTREE__(fp, htr, urls);
                                return NULL;
                            }
                            idx         = (htr->treeidx - 1);
                            htele       = htr->tree[idx];
                            htele->hash = hash;
                            hash_tree_sort_(htr->tree, htr->treeidx);
                            htr->eleidx[0]++;

                            if (plcb != NULL)
                            {
                                plcb(ppath, (void*)&data);
                            }
                            break;
                        }
                        case 0:
                        {
                            htele       = htr->tree[idx];
                            break;
                        }
                    }
                    break;
                }
                 /* case 2: http host or https ip address - URL filter */
                 /* case 3: http url, https is empty - URL filter */
                case HASH_TREE_ELE_HOST:
                case HASH_TREE_ELE_URL:
                {
                    if (htele == NULL)
                    {
                        __MACRO_CLEAR_HASHTREE__(fp, htr, urls);
                        return NULL;
                    }
                    switch (hash_tree_search_(htele->tree, htele->treeidx, hash, &idx))
                    {
                        case -1:
                        {
                            if ((htele->tree = hash_tree_alloc_(htele->tree, &htele->treeidx, htr->eleidx)) == NULL)
                            {
                                __MACRO_CLEAR_HASHTREE__(fp, htr, urls);
                                return NULL;
                            }
                            hash_tree_ele_t *htsort = htele;
                            idx         = (htele->treeidx - 1);
                            htele       = htele->tree[idx];
                            htele->hash = hash;
                            hash_tree_sort_(htsort->tree, htsort->treeidx);
                            htr->eleidx[(i - 1)]++;
                            break;
                        }
                        case 0:
                        {
                            htele       = htele->tree[idx];
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: tree index unsupported element [%d] %s", i, data.str);
                    continue;
                }

            }
            if (urls != NULL)
            {
                free(urls); urls = NULL;
            }
        }
    }
    if (errno != 0)
    {
        __MACRO_TOSYSLOG__(LOG_ERR, STR_HASH_TREE_DEBUG "URL: read %s return: %s", fpath->str, strerror(errno));
    }

    if (sline.str != NULL)
    {
        free(sline.str);
    }

    __MACRO_TOSCREEN__(
        STR_HASH_TREE_DEBUG "URL: read from %s -> %lu ports, %lu hosts, %lu urls.",
            fpath->str,
            htr->eleidx[0],
            htr->eleidx[1],
            htr->eleidx[2]
    );
    __MACRO_CLEAR_HASHTABLE__(fp, NULL);

    if (!hash_tree_check(htr))
    {
        __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: tree index empty.. %s", "Abort..");
        __MACRO_CLEAR_HASHTREE__(-1, htr, NULL);
        return NULL;
    }
    /*
        // DEBUG:
        hash_tree_print(htr->tree, htr->treeidx, 0);
    */
    htrfree = NULL;
    return htr;
}
FUNEXPORT hash_tree_response_e hash_tree_url_search(hash_tree_t *htr, void *obj, int abs_url)
{
    if ((htr == NULL) || (htr->tree == NULL) || (htr->treeidx == 0) || (obj == NULL))
    {
        return HASH_TREE_ERRQUERY;
    }

    __BUFFER_CREATE__(data);
    unsigned long hash, idx = htr->treeidx;
    hash_tree_seach_s *sstr = (hash_tree_seach_s*)obj;
    hash_tree_ele_t **htele = htr->tree;

    for (int i = 0; i < sstr->limit; i++)
    {
        if (sstr->search[i].str == NULL)
        {
            return HASH_TREE_ERRQUERY;
        }

        data.sz =
            ((sstr->search[i].sz > (__BUFFER_SIZE__ - 1)) ?
                (__BUFFER_SIZE__ - 1) : sstr->search[i].sz);

        memcpy((void*)data.str, (void*)sstr->search[i].str, data.sz);
        data.str[data.sz] = '\0';

        switch(i)
        {
            /* URL check is port digits */
            case 0:
            {
                if (!string_isdigit(&data))
                {
                    return HASH_TREE_ERRQUERY;
                }
                break;
            }
            /* URL '//' double slash remove */
            case 2:
            {
                if (
                    (data.sz > 1) &&
                    (data.str[0] == '/') &&
                    (data.str[1] == '/')
                   )
                {
                    string_skipchar(&data, '/');
                }
                break;
            }
        }

        string_trim(&data);
        string_case(&data, 0);
        hash = hash_string_(&data);

        switch (hash_tree_search_(htele, idx, hash, &idx))
        {
            case -1:
            {
                return HASH_TREE_NOTFOUND;
            }
            case 0:
            {
                hash_tree_ele_t **htsel = htele;
                htele = htsel[idx]->tree;
                idx   = htsel[idx]->treeidx;
                if ((i == 1) && (sstr->limit == 2))
                {
                    return ((abs_url) ? HASH_TREE_FOUND :
                        (((htele != NULL) && (htele->treeidx > 0)) ? HASH_TREE_NOTFOUND : HASH_TREE_FOUND));
                }
                continue;
            }
            default:
            {
                return HASH_TREE_ERRQUERY;
            }
        }
    }
    return HASH_TREE_FOUND;
}
FUNEXPORT int hash_tree_check(hash_tree_t *htr)
{
    return (((htr == NULL) || (htr->tree == NULL) || (htr->eleidx[1] == 0)) ? 0 : 1);
}
FUNEXPORT void hash_tree_free(hash_tree_t *htr)
{
    if (
        (htr == NULL) ||
        (htr->treeidx == 0)
       )
    {
        return;
    }
    hash_tree_ele_free_(htr->tree, htr->treeidx);
    free(htr);
}
