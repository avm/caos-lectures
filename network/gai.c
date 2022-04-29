#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NODE SERVICE\n", argv[0]);
        return 1;
    }   
    struct addrinfo *res = NULL;
    int gai_err;
    if ((gai_err = getaddrinfo(argv[1], argv[2], NULL, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return 2;
    }   
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        char host[1024], service[100];
        printf("ai_flags=%d, ai_family=%d, ai_socktype=%d, ai_proto=%d\n",
                ai->ai_flags, ai->ai_family, ai->ai_socktype,
                ai->ai_protocol);

        getnameinfo(ai->ai_addr, ai->ai_addrlen,
            host, sizeof(host), service, sizeof(service),
            NI_NUMERICHOST | NI_NUMERICSERV);
        printf("\taddress: %s, port: %s\n",
            host, service);
            
    }   
    freeaddrinfo(res);
}
