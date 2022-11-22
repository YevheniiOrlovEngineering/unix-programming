#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <log.h>
#include <constants.h>

#define PERMS       0644

void
to_upper(
        char *s
) {
    for (int i = 0; s[i] != '\0'; ++i) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = (const char) (s[i] - 32);
        }
    }
}


int
main (
        int argc, char **argv
        ) {
    const char *bin_name;
    char *p_source = NULL;
    char *p_destination = NULL;
    char buf[BUFSIZ];

    long bytes_total = 0;

    ssize_t bytes_read;
    ssize_t bytes_written;

    int fd_in;
    int fd_out;
    int c;

    opterr = 0;
    errno  = 0;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

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
                return EXIT_FAILURE;
            default:
                return EXIT_FAILURE;
        }

    for (int index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);


    if (-1 == (fd_in = open (p_source, O_RDONLY))) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_source);
        print_log(stderr, bin_name, buf);
        return EXIT_FAILURE;
    }

    if (-1 == (fd_out = open (p_destination, O_CREAT|O_WRONLY, PERMS))) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_destination);
        print_log(stderr, bin_name, buf);
        if (close(fd_in) != 0)
            print_log(stderr, bin_name, strerror (errno));
        return EXIT_FAILURE;
    }

    while ((bytes_read = read (fd_in, buf, BUFSIZ)) > 0 && bytes_read != -1) {
        to_upper(buf);
        if ((bytes_written = write (fd_out, buf, bytes_read)) == -1 || bytes_written < bytes_read) {
            sprintf (buf, ERROR_TPL, strerror (errno), p_destination);
            print_log(stderr, bin_name, buf);
            if (close(fd_in) != 0)
                print_log(stderr, bin_name, strerror (errno));
            if (close(fd_out) != 0)
                print_log(stderr, bin_name, strerror (errno));
            return EXIT_FAILURE;
        }
        bytes_total += bytes_written;
    }

    if (close(fd_in) != 0)
        print_log(stderr, bin_name, strerror (errno));
    if (close(fd_out) != 0)
        print_log(stderr, bin_name, strerror (errno));

    if (-1 == bytes_read) {
        sprintf (buf, ERROR_TPL, strerror (errno), p_source);
        print_log(stderr, bin_name, buf);
        return EXIT_FAILURE;
    } else  {
        sprintf (buf, "Bytes overwritten: %ld", bytes_total);
        print_log(stdout, bin_name, buf);
        return EXIT_SUCCESS;
    }
}
