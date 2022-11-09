#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "constants.h"
#include "log.h"


void
print_process_data(
        const char *type,
        char *buf
        ) {
    pid_t pid = getpid ();
    gid_t gid = getgid ();
    pid_t sid = getsid (0);

    sprintf (buf, "type: %s, pid: %d, gid: %d, sid: %d", type, pid, gid, sid);
    print_log(stdout, buf);
}

int
main(
        int argc, char **argv
        ) {
    const char *p_child = "child";
    const char *p_parent = "parent";
    char buf[BUF_SIZE];

    pid_t pid;

    const size_t child_print_num = 3;
    const size_t sleep_time = 1;
    int status, child_exit_status;

    switch (pid = fork ()) {
        case -1:
            sprintf (buf, ERROR_TPL, strerror (errno), "fork()");
            print_log(stderr, buf);
            break;
        case 0:
            for (size_t i = 0; i < child_print_num; ++i) {
                print_process_data(p_child, buf);
                sleep (sleep_time);
            }
            sprintf(buf, "type: %s. Finished.", p_child);
            print_log(stdout, buf);
            exit (EXIT_SUCCESS);
            break;
        default:
            while(waitpid (pid, &status, WNOHANG) == 0) {
                print_process_data(p_parent, buf);
                sprintf(buf, "type: %s. Waiting %s process to be finished.", p_parent, p_child);
                print_log(stdout, buf);
                sleep (sleep_time);
            }
            if (WIFEXITED(status)) {
                child_exit_status = WEXITSTATUS(status);
                sprintf(buf, "type: %s. %s process (pid: %d) exited normally with %d code",
                        p_parent, p_child, pid, child_exit_status);
                print_log(stdout, buf);
            } else {
                sprintf(buf, "type: %s. %s process (pid: %d) was stopped or finished with some SIGNAL",
                        p_parent, p_child, pid);
                print_log(stdout, buf);
            }
            sprintf(buf, "type: %s. Finished.", p_parent);
            print_log(stdout, buf);
    }
    return EXIT_SUCCESS;
}
