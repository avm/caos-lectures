#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s SIGNAL PID\n", argv[0]);
        return 1;
    }
    if (kill(atoi(argv[2]), atoi(argv[1])) == -1) {
        perror("kill");
        return 1;
    }
}
