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

static int arr_cmpt(const void *a, const void *b)
{
    if (((hash_tree_ele_t*)*(hash_tree_ele_t**)a)->hash <  ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return -1; }
    if (((hash_tree_ele_t*)*(hash_tree_ele_t**)a)->hash == ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return 0;  }
    if (((hash_tree_ele_t*)*(hash_tree_ele_t**)a)->hash >  ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return 1;  }
    return 0;
}
static int arr_cmpts(const void *a, const void *b)
{
    /*
        // DEBUG:
        unsigned long aa = *(unsigned long*)a,
                      bb = ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash;
        printf("\t\t* arr_cmpts: %lu -> %lu\n", aa, bb);
    */

    if (*(unsigned long*)a <  ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return -1; }
    if (*(unsigned long*)a == ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return 0;  }
    if (*(unsigned long*)a >  ((hash_tree_ele_t*)*(hash_tree_ele_t**)b)->hash) { return 1;  }
    return 0;
}
static void hash_tree_sort_(hash_tree_ele_t **hte, unsigned long idx)
{
    qsort((void*)hte, idx, sizeof(hash_tree_ele_t*), &arr_cmpt);
}
static hash_tree_ele_t * hash_tree_search_(hash_tree_ele_t *hte, unsigned long hash)
{
    if ((hte == NULL) || (hash == 0UL)) { return NULL; }
    hash_tree_ele_t **heo;
    if ((heo = bsearch(
        (const void*)&hash,
        (const void*)hte->tree,
        hte->treeidx,
        sizeof(hash_tree_ele_t*),
        &arr_cmpts)
    ) == NULL) { return NULL; }
    return *heo;
}

//////////////////////////

static hash_tree_ele_t * hash_tree_alloc_(hash_tree_ele_t *htele, unsigned long *cnt)
{
    do {
        if (htele == NULL)
        {
            return NULL;
        }
        if (htele->tree == NULL)
        {
            if ((htele->tree                 = calloc(1, sizeof(hash_tree_ele_t*))) == NULL) { break; }
            if ((htele->tree[htele->treeidx] = calloc(1, sizeof(hash_tree_ele_t))) == NULL)  { break; }
            htele->treeidx = 1;
        }
        else
        {
            if ((htele->tree = realloc(htele->tree, ((htele->treeidx + 1) * sizeof(hash_tree_ele_t*)))) == NULL) { break; }
            if ((htele->tree[htele->treeidx] = calloc(1, sizeof(hash_tree_ele_t))) == NULL)  { break; }
            htele->treeidx++;
        }
        if ((cnt[1] != 0UL) && ((cnt[1] % 100) == 0))
        {
            __MACRO_TOSCREEN__(STR_HASH_TBL_DEBUG "URL complette parse -> %lu hosts", cnt[1]);
        }
        return htele->tree[(htele->treeidx - 1)];

    } while(0);

    if (htele != NULL)
    {
        hash_tree_ele_free_(htele->tree, htele->treeidx);
        free(htele);
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
        if ((sline.sz < 2) || (sline.str == NULL) || (sline.str[0] == '#'))
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

        hash_tree_ele_t *htele = NULL, *htsearch = NULL;
        unsigned long hash = 0UL;
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
            /*
                // DEBUG:
                printf("\t* hash_tree_url_read:%d [%s] [%lu]\n", i, data.str, hash);
            */

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
                    htele = &htr->base;
                    break;
                }
                 /* case 2: http host or https ip address - URL filter */
                 /* case 3: http url, https is empty - URL filter */
                case HASH_TREE_ELE_HOST:
                case HASH_TREE_ELE_URL:
                {
                    if (htele == NULL)
                    {
                        __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: tree root element empty! [%d] %s", i, data.str);
                        __MACRO_CLEAR_HASHTREE__(fp, htr, urls);
                        return NULL;
                    }
                    break;
                }
                default:
                {
                    __MACRO_TOSCREEN__(STR_HASH_TREE_DEBUG "URL: tree index unsupported element [%d] %s", i, data.str);
                    continue;
                }
            }
            if ((htsearch = hash_tree_search_(htele, hash)) == NULL)
            {
                if ((htsearch = hash_tree_alloc_(htele, htr->eleidx)) == NULL)
                {
                    __MACRO_CLEAR_HASHTREE__(fp, htr, urls);
                    return NULL;
                }
                memcpy(&htsearch->hash, &hash, sizeof(unsigned long));
                hash_tree_sort_(htele->tree, htele->treeidx);
                htele = htsearch; htsearch = NULL;
                htr->eleidx[(i - 1)]++;
                if ((i == HASH_TREE_ELE_PORT) && (plcb != NULL))
                {
                    plcb(ppath, (void*)&data);
                }
            }
            else
            {
                htele       = htsearch;
                htsearch    = NULL;
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
        hash_tree_print(htr->base.tree, htr->base.treeidx, 0);
    */

    htrfree = NULL;
    return htr;
}
FUNEXPORT hash_tree_response_e hash_tree_url_search(hash_tree_t *htr, void *obj, int abs_url)
{
    if ((htr == NULL) || (htr->base.tree == NULL) || (htr->base.treeidx == 0) || (obj == NULL))
    {
        return HASH_TREE_ERRQUERY;
    }

    __BUFFER_CREATE__(data);
    unsigned long hash;
    hash_tree_seach_s *sstr = (hash_tree_seach_s*)obj;
    hash_tree_ele_t *htele = &htr->base;

    for (int i = 0; i < sstr->limit; i++)
    {
        if (sstr->search[i].str == NULL)
        {
            return HASH_TREE_ERRQUERY;
        }
        if (htele == NULL)
        {
            return HASH_TREE_NOTFOUND;
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

        if ((htele = hash_tree_search_(htele, hash)) == NULL)
        {
            return HASH_TREE_NOTFOUND;
        }
        else
        {
            if ((i == 1) && (sstr->limit == 2))
            {
                return ((abs_url) ? HASH_TREE_FOUND :
                    ((htele->treeidx > 0) ? HASH_TREE_NOTFOUND : HASH_TREE_FOUND));

            }
        }
    }
    return HASH_TREE_FOUND;
}
FUNEXPORT int hash_tree_check(hash_tree_t *htr)
{
    return (((htr == NULL) || (htr->base.tree == NULL) || (htr->eleidx[1] == 0)) ? 0 : 1);
}
FUNEXPORT void hash_tree_free(hash_tree_t *htr)
{
    if (
        (htr == NULL) ||
        (htr->base.treeidx == 0)
       )
    {
        return;
    }
    hash_tree_ele_free_(htr->base.tree, htr->base.treeidx);
    free(htr);
}
