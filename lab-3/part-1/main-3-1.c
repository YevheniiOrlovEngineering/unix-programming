#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "log.h"
#include "file.h"

static int fd;
static const char* bin_name;


int
open_log(const char *path, int oflag) {
    if ((fd = open (path, oflag, 0644)) == -1) {
        print_log(stderr, bin_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    return fd;
}



void
signal_handler(int signo, siginfo_t* si, void* ucontext) {
    char buf[BUFSIZ];

    sprintf(buf, "SIGHUP signal\n");
    write_to_file(fd, buf, bin_name);

    sprintf(buf, "Sigaction:\n\t"
                 "si_signo: %d\n\t"
                 "si_errno: %d\n\t"
                 "si_code: %d\n\t"
                 "si_pid: %d\n\t"
                 "si_uid: %d\n\t"
                 "si_status: %d\n\t"
                 "si_utime: %ld\n\t"
                 "si_stime: %ld\n\t"
                 "si_value.sival_int: %d\n\t"
                 "si_int: %d\n\t"
                 "si_fd: %d\n\t"
                 "si_pkey: %d\n\t"
                 "si_syscall: %d\n\t"
                 "si_arch: %d\n",
                 si->si_signo,
                 si->si_errno,
                 si->si_code,
                 si->si_pid,
                 si->si_uid,
                 si->si_status,
                 si->si_utime,
                 si->si_stime,
                 si->si_value.sival_int,
                 si->si_int,
                 si->si_fd,
                 si->si_pkey,
                 si->si_syscall,
                 si->si_arch);

    write_to_file(fd, buf, bin_name);

    if (signal(signo, SIG_DFL) == SIG_ERR) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (raise(signo) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int
main(int argc, char** argv) {
    struct sigaction act;
    const char *fn_log = "/lab-3-1.log";
    char full_fn_log[BUFSIZ];
    char buf[BUFSIZ];
    unsigned sleeping_time = 5;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    if (getcwd(buf, sizeof(buf)) != NULL) {
        strcat(buf, fn_log);
        strcpy(full_fn_log, buf);
    } else {
        print_log(stderr, bin_name, strerror(errno));
        return EXIT_FAILURE;
    }

    fd = open_log(full_fn_log, O_CREAT | O_WRONLY | O_TRUNC);

    sprintf(buf, "%s %s: Main process started. pid: %d\n",
            get_cur_time(), bin_name, getpid());
    write_to_file(fd, buf, bin_name);

    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = &signal_handler;
    if(sigaction(SIGHUP, &act, NULL) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        return EXIT_FAILURE;
    }

    for (;;) {
        sprintf(buf, "Main process. pid: %d. Sleeping for %u seconds\n", getpid(), sleeping_time);
        write_to_file(fd, buf, bin_name);
        sleep(sleeping_time);
    }
    return EXIT_SUCCESS;
}
