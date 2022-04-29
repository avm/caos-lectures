#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void handler(int sig) {
    char msg[] = "handler running\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    alarm(1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s SECONDS\n", argv[0]);
        return 1;
    }
    int seconds = atoi(argv[1]);
    signal(SIGALRM, handler);
    while (1) {
        printf("Setting an alarm for %d seconds\n", seconds);
        alarm(seconds);
        printf("Reading from STDIN\n");
        char buf[1024];
        int res = read(STDIN_FILENO, buf, sizeof(buf));
        printf("read returned %d\n", res);
    }
}
