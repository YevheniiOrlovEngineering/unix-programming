#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "constants.h"
#include "log.h"

static const char *bin_name;

void
print_process_data(
        const char *type,
        char *buf
        ) {
    pid_t pid = getpid ();
    gid_t gid = getgid ();
    pid_t sid = getsid (0);

    sprintf (buf, "type: %s, pid: %d, gid: %d, sid: %d", type, pid, gid, sid);
    print_log(stdout, bin_name, buf);
}

int
main(
        int argc, char **argv
        ) {
    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    const char *p_child = "child";
    const char *p_parent = "parent";
    char buf[BUFSIZ];

    pid_t pid;

    const size_t child_print_num = 3;
    const size_t sleep_time = 1;
    int status, child_exit_status;

    switch (pid = fork ()) {
        case -1:
            print_log(stderr, bin_name, strerror (errno));
            break;
        case 0:
            for (size_t i = 0; i < child_print_num; ++i) {
                print_process_data(p_child, buf);
                sleep (sleep_time);
            }
            sprintf(buf, "type: %s. Finished.", p_child);
            print_log(stdout, bin_name, buf);
            exit (EXIT_SUCCESS);
            break;
        default:
            while(waitpid (pid, &status, WNOHANG) == 0) {
                print_process_data(p_parent, buf);
                sprintf(buf, "type: %s. Waiting %s process to be finished.", p_parent, p_child);
                print_log(stdout, bin_name, buf);
                sleep (sleep_time);
            }
            if (WIFEXITED(status)) {
                child_exit_status = WEXITSTATUS(status);
                sprintf(buf, "type: %s. %s process (pid: %d) exited normally with %d code",
                        p_parent, p_child, pid, child_exit_status);
                print_log(stdout, bin_name, buf);
            } else {
                sprintf(buf, "type: %s. %s process (pid: %d) was stopped or finished with some SIGNAL",
                        p_parent, p_child, pid);
                print_log(stdout, bin_name, buf);
            }
            sprintf(buf, "type: %s. Finished.", p_parent);
            print_log(stdout, bin_name, buf);
    }
    return EXIT_SUCCESS;
}
