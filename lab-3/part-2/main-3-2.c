#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include "log.h"

struct datum {
    int pid;
    time_t time;
    char value[BUFSIZ];
};

int
main (int argc, char** argv) {
    struct datum *p_datum;
    const char* bin_name;
    char buf[BUFSIZ];
    int fd;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    if ((fd = shm_open("/Orlov", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(struct datum)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((p_datum = mmap(NULL, sizeof(*p_datum), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(;;) {
        print_log(stdout, bin_name, "Enter the value: ");
        fgets(buf, sizeof(buf), stdin);

        if(msync(p_datum, sizeof(struct datum), MS_SYNC) == -1) {
            print_log(stderr, bin_name, strerror(errno));
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "pid = %d\ttime = %ld\tvalue = %s", p_datum->pid, p_datum->time, p_datum->value);

        p_datum->pid = getpid();
        p_datum->time = time(NULL);
        strcpy(p_datum->value, buf);
    }
}
