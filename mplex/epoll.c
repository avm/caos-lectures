#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>


void child(int* pipe) {
    int counter = 0;
    printf("%d: starting\n", getpid());
    close(pipe[0]);
    while (1) {
        sleep(1);
        dprintf(pipe[1], "%d %d\n", getpid(), counter++); 
    }
}


void fork_child(int* pipes, size_t i, int epollfd) {
    pipe(pipes + i*2);
    if (!fork()) {
        child(pipes + i*2);
        _exit(0);
    }
    int readfd = pipes[i*2];
    struct epoll_event evt = {.events = EPOLLIN, .data.fd = readfd};
    epoll_ctl(epollfd, EPOLL_CTL_ADD, readfd, &evt);
}


sig_atomic_t need_fork;


void handler(int sig) {
    need_fork = 1;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        exit(1);
    }
    size_t N = atoi(argv[1]);
    int pipes[N*2];
    int existing_children = 0;

    signal(SIGUSR1, handler);

    sigset_t sigset, oldmask;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, &oldmask);

    int epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perror("epoll");
        return 1;
    }
    while(1) {
        if (need_fork) {
            fork_child(pipes, existing_children++, epollfd);
            need_fork = 0;
        }

        struct epoll_event evt;

        int timeout = -1; // block indefinitely
        errno = 0;
        if (epoll_pwait(epollfd, &evt, 1, timeout, &oldmask) < 1) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            return 1;
        }
        
        char buf[1024] = {0};
        read(evt.data.fd, buf, sizeof(buf) - 1);
        printf("fd %d: %s\n", evt.data.fd, buf);
    }
}
