#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s DELAY\n", argv[0]);
        return 1;
    }
    printf("%s/%d: sleeping\n", argv[0], getpid());
    sleep(atoi(argv[1]));

    printf("%s/%d: reading from stdin\n", argv[0], getpid());
    char buf[1024];
    ssize_t r = read(STDIN_FILENO, buf, sizeof(buf));
    printf("%s/%d: read %d bytes\n", argv[0], getpid(), r);
}
