#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <constants.h>
#include <log.h>

int
main (
        int argc, char **argv
        ) {
    char *par = NULL;
    int c;

    opterr = 0;
    errno  = 0;

    while ((c = getopt (argc, argv, "p:")) != -1)
        switch (c) {
            case 'p':
                par = optarg;
                break;
            case '?':
                if (optopt == 'p')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return EXIT_FAILURE;
            default:
                return EXIT_FAILURE;
        }

    for (int index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    char read_buf[BUF_SIZE];
    char write_buf[2 * BUF_SIZE];

    if (par == NULL) {
        sprintf (write_buf, ERROR_TPL, "No string identifier had been passed", par);
        print_log(stdout, LOG_FMT_TPL, LOG_PARAMS, write_buf);
        return EXIT_FAILURE;
    }

    fd_set rfds;
    int ndfs = STDIN_FILENO + 1;

    ssize_t bytes_read;
    ssize_t bytes_written;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);

        struct timeval tv = { tv.tv_sec = 5, tv.tv_usec = 0 };

        switch (select(ndfs, &rfds, NULL, NULL, &tv)) {
            case -1:
                print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, strerror(errno));
                break;
            case 0:
                sprintf (write_buf, ERROR_TPL,  "Time's out. No data has been passed", par);
                print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, write_buf);
                break;
            default:
                if ((bytes_read = read (STDIN_FILENO, read_buf, BUF_SIZE)) > 0 && bytes_read != -1) {
                    sprintf(write_buf, "%s", par);
                    print_log(stdout, LOG_FMT_TPL, LOG_PARAMS, write_buf);
                    if ((
                            bytes_written = write (STDOUT_FILENO, read_buf, bytes_read)) == -1 ||
                            bytes_written < bytes_read) {
                        print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, strerror (errno));
                        return EXIT_FAILURE;
                    }
                } else {
                    print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, strerror (errno));
                    return EXIT_FAILURE;
                }
        }
    }
}