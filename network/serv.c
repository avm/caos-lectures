#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int create_listener(char* service, int* family) {
    struct addrinfo *res = NULL;
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
    };
    if (gai_err = getaddrinfo(NULL, service, &hint, &res)) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return -1;
    }
    int sock = -1;
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }
        int one = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
            perror("setsockopt");
        }
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("bind");
            close(sock);
            sock = -1;
            continue;
        }
        if (listen(sock, 1) < 0) {
            perror("listen");
            close(sock);
            sock = -1;
            continue;
        }
        *family = ai->ai_family;
        break;
    }
    freeaddrinfo(res);
    return sock;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s SERVICE\n", argv[0]);
        return 1;
    }
    int ai_family;
    int sock = create_listener(argv[1], &ai_family);
    if (sock < 0) {
        return 1;
    }
    while (1) {
        struct sockaddr_storage client;
        socklen_t len = sizeof(client);
        int connection = accept(sock, (struct sockaddr *)&client, &len);
        char host[100], port[10];
        getnameinfo((struct sockaddr *)&client, len,
                    host, sizeof(host), port, sizeof(port),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        printf("\taddress: %s, port: %s\n", host, port);
        if (!fork()) {
            char* msg = "hello world\n";
            write(connection, msg, strlen(msg));
            return 0;
        }
        close(connection);
        while (wait(NULL) != -1);
    }
}
