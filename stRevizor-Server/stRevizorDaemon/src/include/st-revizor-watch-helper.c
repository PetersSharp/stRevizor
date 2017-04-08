
#define __STREVIZOR_GIT_REPO "https://github.com/PetersSharp/stRevizor/releases/latest"
#define __STREVIZOR_FOOTER_STR "\n\tRun %s -h for more information.\n\n"

#if defined(PACKAGE_STRING)
#   define __STREVIZOR_HEAD_STR \
        "\tBinary:\t\t\t%s\n" \
        "\tstRevizor version:\t" PACKAGE_STRING "\n" \
        "\tGit latest: " __STREVIZOR_GIT_REPO "\n\n"
#else
#   define __STREVIZOR_HEAD_STR \
        "\tBinary:\t\t\t%s\n" \
        "\tGit latest: " __STREVIZOR_GIT_REPO "\n\n"
#endif

    if (argc > 1)
    {
        if ((strlen(argv[1]) == 2) && (argv[1][0] == '-'))
        {
            if (
                 (argv[1][1] == 'h') ||
                 (argv[1][1] == 'H') ||
                 (argv[1][1] == '?')
               )
            {
                printf(__STREVIZOR_HEAD_STR, argv[0]);
                printf(
                    "\t\t-h this information.\n" \
                    "\t\t-i build information.\n" \
                    "\t\t-e environment information.\n" \
                    "\t\t-v version information.\n"
                    "\t\t-c ipfw check script.\n"
                    "\n\t\tRun standalone:\n" \
                    "\t\t%s <watch dir1> <watch dir2> .. <watch dirX>\n" \
                    "\t\tor use setup default input directory:\n" \
                    "\t\t%s\n",
                    argv[0], argv[0]
                );
                printf(__STREVIZOR_FOOTER_STR, argv[0]);
                _exit(0);
            }
            else if (
                 (argv[1][1] == 'i') ||
                 (argv[1][1] == 'I')
               )
            {

                char rbuf[65536] = {0};
                const size_t buildl = (builde - builds);
                memcpy(CP rbuf, builds, buildl);
                printf(__STREVIZOR_HEAD_STR, argv[0]);
                printf("\tBuild information:\n\n%s\n", rbuf);
                printf(__STREVIZOR_FOOTER_STR, argv[0]);
                _exit(0);
            }
            else if (
                 (argv[1][1] == 'v') ||
                 (argv[1][1] == 'V')
               )
            {
                printf(__STREVIZOR_HEAD_STR, argv[0]);
                printf("\tBuild date:\t\t%s %s\n", __DATE__, __TIME__);
                printf(__STREVIZOR_FOOTER_STR, argv[0]);
                _exit(0);
            }
            else if (
                 (argv[1][1] == 'e') ||
                 (argv[1][1] == 'E')
               )
            {
                printf(__STREVIZOR_HEAD_STR, argv[0]);
                printf("\tstRevizor IPFW environment:\n\n");
                ipfw_env_prn();
                printf(__STREVIZOR_FOOTER_STR, argv[0]);
                _exit(0);
            }
            else if (
                 (argv[1][1] == 'c') ||
                 (argv[1][1] == 'C')
               )
            {
                printf(__STREVIZOR_HEAD_STR, argv[0]);
                printf("\tstRevizor IPFW check script:\n\n");
                ipfw_pack_check();
                printf(__STREVIZOR_FOOTER_STR, argv[0]);
                _exit(0);
            }
        }
    }
