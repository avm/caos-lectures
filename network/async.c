#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>

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
        sock = socket(ai->ai_family, ai->ai_socktype, IPPROTO_TCP);
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
        if (listen(sock, SOMAXCONN) < 0) {
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
    int epollfd = epoll_create1(0);
    enum { MAX_EVENTS = 1 };
    struct epoll_event ev, events[MAX_EVENTS];
    
    // Add listening socket to epollfd
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev);

    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        
        if (nfds != 1) {
            abort();
        }

        if (events[0].data.fd == sock) {
            struct sockaddr_storage client;
            socklen_t len = sizeof(client);
            int connection = accept(sock, (struct sockaddr *)&client, &len);
            char host[100], port[10];
            getnameinfo((struct sockaddr *)&client, len,
                        host, sizeof(host), port, sizeof(port),
                        NI_NUMERICHOST | NI_NUMERICSERV);
            printf("\taddress: %s, port: %s\n", host, port);
            ev.events = EPOLLIN;
            ev.data.fd = connection;
            epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &ev);
        } else if (events[0].events & EPOLLIN) {
            char buf[1024];
            ssize_t chunk = read(events[0].data.fd, buf, sizeof(buf)-1);
            if (chunk == 0) {
                printf("EOF on %d, closing it\n", events[0].data.fd);
                close(events[0].data.fd);
            } else {
                buf[chunk] = '\0';
                printf("read from %d: %s\n", events[0].data.fd, buf);

                // write(events[0].data.fd, buf, chunk);
                ev.events = EPOLLOUT;
                ev.data.fd = events[0].data.fd;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, events[0].data.fd, &ev);
            }
        } else {
            assert(events[0].events & EPOLLOUT);
            write(events[0].data.fd, "ack\n", 4);
            ev.events = EPOLLIN;
            ev.data.fd = events[0].data.fd;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, events[0].data.fd, &ev);
        }
    }
}
