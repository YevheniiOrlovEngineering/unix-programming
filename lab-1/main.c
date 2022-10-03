#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "../utils/log/log.h"
#include "../utils/file/file.h"

#define BUF_SIZE    512
#define PERMS       0644
#define LOG_PARAMS   __DATE__, __FILE__, __FUNCTION__, __LINE__
#define LOG_FMT_TPL "[ %s ] [ SOURCE: %s | FUNCTION: %s() | LINE: %d ]\n(%s) \n"
#define ERROR_TPL   "%s: %s"




void
close_file(
        const int fd, const char *f_name
        ) {
    char msg_buf[BUF_SIZE];
    if (-1 == close (fd)) {
        sprintf(msg_buf, ERROR_TPL, strerror (errno), f_name);
        print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, msg_buf);
    }
}


int
main (
        int argc, char **argv
        ) {
    char *p_source = NULL;
    char *p_destination = NULL;
    char buf[BUF_SIZE];

    long bytes_total = 0;

    ssize_t bytes_read;
    ssize_t bytes_written;

    int fd_in;
    int fd_out;
    int c;

    opterr = 0;
    errno  = 0;

    while ((c = getopt (argc, argv, "i:o:")) != -1)
        switch (c) {
            case 'i':
                p_source = optarg;
                break;
            case 'o':
                p_destination = optarg;
                break;
            case '?':
                if ((optopt == 'i') || (optopt == 'o'))
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }

    for (int index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    if (-1 == (fd_in = open (p_source, O_RDONLY))) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_source);
        print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, buf);
        exit (EXIT_FAILURE);
    }

    if (-1 == (fd_out = open (p_destination, O_CREAT|O_WRONLY, PERMS))) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_destination);
        print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, buf);
        close_file(fd_in, p_source);
        exit (EXIT_FAILURE);
    }

    while ((bytes_read = read (fd_in, buf, BUF_SIZE)) > 0 && bytes_read != -1) {
        to_upper(buf);
        if ((bytes_written = write (fd_out, buf, bytes_read)) == -1) {
            sprintf (buf, ERROR_TPL, strerror (errno), p_destination);
            print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, buf);
            close_file(fd_in, p_source);
            close_file(fd_out, p_destination);
            exit (EXIT_FAILURE);
        } else if (bytes_written < bytes_read) {
            sprintf (buf, ERROR_TPL, strerror (errno), p_destination);
            print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, buf);
            close_file(fd_in, p_source);
            close_file(fd_out, p_destination);
            exit (EXIT_FAILURE);
        }
        bytes_total += bytes_written;
    }

    close_file(fd_in, p_source);
    close_file(fd_out, p_destination);

    if (-1 == bytes_read) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_source);
        print_log(stderr, LOG_FMT_TPL, LOG_PARAMS, buf);
        exit (EXIT_FAILURE);
    } else  {
        sprintf (buf, "Bytes overwritten: %ld", bytes_total);
        print_log(stdout, LOG_FMT_TPL, LOG_PARAMS, buf);
        exit (EXIT_SUCCESS);
    }
}

/*
 * QA:
 * eval file name, having file descriptor
 * logging standard in C
 * sprintf usage when no numbers; (want to expand values to template)
 * use char* instead of char[] in close file
 */