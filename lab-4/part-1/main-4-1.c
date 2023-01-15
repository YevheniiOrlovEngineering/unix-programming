#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "log.h"
#include "file.h"

#define PORT_NUMBER 3212
#define MAX_CLIENT_NUMBER 5

static int sock = 0;

void handler() {
    close(sock);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    struct sockaddr_in server_sockaddr_struct = {
            .sin_family = PF_INET,
            .sin_addr.s_addr = htonl(INADDR_ANY),
            .sin_port = htons(PORT_NUMBER),
    };

    struct sigaction sigact_struct;
    sigact_struct.sa_handler = handler;

    const char* bin_name;
    char buf[3*BUFSIZ];
    pid_t pid;
    int _socket;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    int fd = open_file("lab-4-1.log", O_WRONLY | O_CREAT | O_TRUNC, bin_name);

    sprintf(buf, "%s %s: Main process started. pid: %d, gid: %d, ppid: %d, sid: %d\n",
            get_cur_time(), bin_name, getpid (), getgid (), getppid (), getsid (getpid ()));
    write_to_file(fd, buf, bin_name);

    umask(0);
    if ((pid = fork ()) < 0) {
        print_log(stderr, bin_name, strerror(errno));
        if (close (fd) != 0)
            print_log(stderr, bin_name, strerror(errno));
        return EXIT_FAILURE;
    } else if (pid != 0) {
        sprintf(buf, "Parent process finishing. pid: %d. gid: %d. sid: %d\n",
                getpid(), getgid(), getsid(getpid()));
        write_to_file(fd, buf, bin_name);
        exit(EXIT_SUCCESS);
    }

    setsid ();

    if ((pid = fork ()) < 0) {
        print_log(stderr, bin_name, strerror(errno));
        if (close (fd) != 0)
            print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        sprintf(buf, "Second fork is done to release process as session leader. "
                     "pid: %d. gid: %d. sid: %d\n",
                getpid(), getgid(), getsid(getpid()));
        write_to_file(fd, buf, bin_name);
        exit(EXIT_SUCCESS);
    }

    if (chdir ("/") < 0)
        print_log(stderr, bin_name, strerror(errno));

    sprintf(buf, "Daemon process started. pid: %d. gid: %d. sid: %d\n",
            getpid(), getgid(), getsid(getpid()));
    write_to_file(fd, buf, bin_name);

    sigfillset(&(sigact_struct.sa_mask));
    sigaction(SIGPIPE, &sigact_struct, NULL);

    if ((_socket = socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    write_to_file(fd, "Socket created\n", bin_name);

    if (bind(_socket, (struct sockaddr *)&server_sockaddr_struct, sizeof(server_sockaddr_struct)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    write_to_file(fd, "Address has been binded with socket\n", bin_name);

    if (listen(_socket, MAX_CLIENT_NUMBER) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    write_to_file(fd, "Started accepting connections\n", bin_name);

    for (;;) {
        if ((sock = accept(_socket, NULL, NULL)) == -1) {
            print_log(stderr, bin_name, strerror(errno));
            continue;
        }

        if (fork() == 0) {
            sprintf(buf, "Accepted connection. Created child process. pid: %d. gid: %d. sid: %d\n",
                    getpid(), getgid(), getsid(getpid()));
            write_to_file(fd, buf, bin_name);

            char receiver[2*BUFSIZ];
            time_t current_time;
            time(&current_time);
            struct tm *local = localtime(&current_time);
            char tmp_buffer[BUFSIZ];

            while (recv(sock, receiver, BUFSIZ, 0) > 0) {
                if (strcmp(receiver, "close") == 0) {
                    send(sock, receiver, BUFSIZ, 0);
                    break;
                }
                strcpy(tmp_buffer, receiver);
                sprintf(receiver, "%02d:%02d:%02d_%d: %s\n",
                        local->tm_hour, local->tm_min, local->tm_sec, getpid(), tmp_buffer);
                if (sprintf(buf, "Received message -> %s", receiver) == -1) {
                    exit(EXIT_FAILURE);
                }
                write_to_file(fd, buf, bin_name);

                send(sock, receiver, BUFSIZ, 0);
            }

            write_to_file(fd, "Child process has been finished\n", bin_name);

            close(sock);
            exit(EXIT_SUCCESS);
        }
        close(sock);
    }
}
