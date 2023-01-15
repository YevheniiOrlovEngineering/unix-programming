#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.h"

#define PORT_NUMBER 3212

int
main(int argc, char** argv) {
    char buf[2*BUFSIZ];
    char receiver[BUFSIZ];
    const char* bin_name;
    int sock;

    if (argc == 0) {
        print_log(stderr, "Unknown", "Program name had not been passed as cmd arg.");
        return EXIT_FAILURE;
    }
    bin_name = argv[0];

    struct sockaddr_in server_sockaddr_struct = {
            .sin_family = PF_INET,
            .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
            .sin_port = htons(PORT_NUMBER),
    };

    if ((sock = socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_sockaddr_struct, sizeof(struct sockaddr_in)) == -1) {
        print_log(stderr, bin_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    print_log(stdout, bin_name, "Client connected to the server successfully");
    while (strcmp(buf, "close") != 0) {
        print_log(stdout, bin_name, "Please, enter a message -> ");
        scanf("%s", buf);
        send(sock, buf, BUFSIZ, 0);
        if (recv(sock, receiver, BUFSIZ, 0) > 0) {
            sprintf(buf, "Received message from server. %s", receiver);
            print_log(stdout, buf, bin_name);
        }
        else {
            print_log(stderr, bin_name, strerror(errno));
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
