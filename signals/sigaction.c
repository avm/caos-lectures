#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

void handler(int sig, siginfo_t *si, void *unused) {
    _Exit(3);
}

int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
    }
    uintptr_t i = 0x42;
    char *str = (char *)i;
    puts(str);
}
