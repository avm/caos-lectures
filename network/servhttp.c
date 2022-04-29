#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
    if ((gai_err = getaddrinfo(NULL, service, &hint, &res))) {
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
            char *method, *path, *protocol;
            int wconn = dup(connection);
            FILE* rfile = fdopen(connection, "r");
            FILE* wfile = fdopen(wconn, "w");
            if (fscanf(rfile, "%ms %ms %ms", &method, &path, &protocol) != 3) {
                return 1;
            }
            int requested_file = open(path + 1, O_RDONLY);
            if (requested_file < 0) {
                fprintf(wfile, "HTTP/1.0 404 Not Found\r\n\r\n");
                return 1;
            }
            fprintf(wfile, "HTTP/1.0 200 Ok\r\n\r\n");
            char buf[1024];
            ssize_t chunk;
            while ((chunk = read(requested_file, buf, sizeof(buf))) > 0) {
                fwrite(buf, 1, chunk, wfile);
            }
            fflush(wfile);
            fclose(rfile);
            return 0;
        }
        close(connection);
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}
