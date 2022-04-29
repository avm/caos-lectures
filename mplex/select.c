#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>


void child(int* pipe) {
    int counter = 0;
    close(pipe[0]);
    while (1) {
        sleep(1);
        dprintf(pipe[1], "%d %d\n", getpid(), counter++); 
    }
}


void fork_child(int* pipes, size_t i) {
    pipe(pipes + i*2);
    if (!fork()) {
        child(pipes + i*2);
        _exit(0);
    }
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

    fd_set readfds;
    while(1) {
        if (need_fork) {
            fork_child(pipes, existing_children++);
            need_fork = 0;
        }
        FD_ZERO(&readfds);
        int max = 0;
        for (size_t i = 0; i < existing_children; i++) {
            int fd = pipes[i*2];
            FD_SET(fd, &readfds);
            max = max < fd ? fd : max;
        }
        struct timespec tv;
        tv.tv_sec = 10;
        tv.tv_nsec = 0;

        if (pselect(max + 1, &readfds, NULL, NULL, &tv, &oldmask) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            return 1;
        }
        for (size_t i = 0; i < existing_children; i++) {
            int fd = pipes[i*2];
            if (FD_ISSET(fd, &readfds)) {
                char buf[1024] = {0};
                read(fd, buf, sizeof(buf) - 1);
                printf("%s (%ld sec %ld nsec left)\n", buf, tv.tv_sec, tv.tv_nsec);
            }
        }
    }
}
