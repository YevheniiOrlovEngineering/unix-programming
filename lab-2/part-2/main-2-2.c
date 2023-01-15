#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "log.h"
#include "file.h"


int
main(int argc, char **argv) {
    const char *bin_name;
    const char *fn_log = "/events.log";
    char full_fn_log[BUFSIZ];
    char buf[BUFSIZ];
    pid_t pid;
    ssize_t bytes_written;
    int fd;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    if (getcwd (buf, sizeof (buf)) != NULL) {
        strcat (buf, fn_log);
        strcpy (full_fn_log, buf);
    } else {
        print_log(stderr, bin_name, strerror (errno));
        return EXIT_FAILURE;
    }

    fd = open_file(full_fn_log, O_CREAT | O_WRONLY | O_TRUNC, bin_name);

    sprintf(buf, "%s %s: Main process started. pid: %d, gid: %d, ppid: %d, sid: %d\n",
            get_cur_time(), bin_name, getpid (), getgid (), getppid (), getsid (getpid ()));
    write_to_file(fd, buf, bin_name);

    umask(0);
    if ((pid = fork ()) < 0) {
        print_log(stderr, bin_name, strerror (errno));
        if (close (fd) != 0)
            print_log(stderr, bin_name, strerror (errno));
        return EXIT_FAILURE;
    } else if (pid != 0)
        exit (EXIT_SUCCESS);

    setsid ();

    // Second fork() is done as a good practice,
    // to release a process as the session leader

    if ((pid = fork ()) < 0) {
        print_log(stderr, bin_name, strerror (errno));
        if (close (fd) != 0)
            print_log(stderr, bin_name, strerror (errno));
        exit (EXIT_FAILURE);
    } else if (pid != 0)
        exit (EXIT_SUCCESS);

    if (chdir ("/") < 0)
        print_log(stderr, bin_name, strerror (errno));

    for (int i = 0; i < sysconf (_SC_OPEN_MAX); i++)
        close (i);

    open ("/dev/null", O_WRONLY);
    dup (0);
    dup (0);

    fd = open_file(full_fn_log, O_WRONLY|O_APPEND, bin_name);

    for (;;) {
        sprintf(buf, "%s %s: Daemon Started. pid: %d, gid: %d, ppid: %d, sid: %d\n",
                get_cur_time(), bin_name, getpid (), getgid (), getppid (), getsid (getpid ()));
        write_to_file(fd, buf, bin_name);
        sleep (1);
    }
}
