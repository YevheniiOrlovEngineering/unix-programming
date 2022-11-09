#include <file.h>

void
close_file(
        const int fd, const char *f_name
) {
    char msg_buf[BUF_SIZE];
    if (-1 == close (fd)) {
        sprintf (msg_buf, ERROR_TPL, strerror (errno), f_name);
        print_log(stderr, msg_buf);
    }
}
