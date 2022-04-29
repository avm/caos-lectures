#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s DELAY STRING\n", argv[0]);
        return 1;
    }
    printf("%s: my pid is %d\n", argv[0], getpid());
    int delay = atoi(argv[1]);
    int counter = 1;
    for (;;) {
        sleep(delay);
        printf("[%d] %d: %s\n", getpid(), counter++, argv[2]);
    }
}
