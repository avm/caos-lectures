#include <signal.h>
#include <unistd.h>
#include <stdio.h>

volatile sig_atomic_t counter;

void handler(int sig) {
    signal(SIGINT, handler);
    char msg[] = "handler running\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    if (++counter > 2) {
        _exit(0);
    }
}

int main(int argc, char* argv[]) {
    printf("%s: my pid is %d\n", argv[0], getpid());
    signal(SIGINT, handler);
    signal(SIGTERM, SIG_IGN);
    int bottles = 99;
    printf("current stack is around %p\n", &bottles);
    for (;;) {
        printf("%d bottles of beer on the wall\n", bottles--);
        sleep(10);
    }
}
