#include "file.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "log.h"

void write_to_file(const int fd, const char* buf, const char* bin_name) {
    ssize_t bytes_written;
    if ((bytes_written = write(fd, buf, strlen(buf))) != strlen(buf) || bytes_written < strlen(buf)) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
