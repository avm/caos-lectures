#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

const char* strings[] = {
    "there once was a ship",
    "that put to sea",
    "and the name",
    "of that ship was the Billy O'Tea",
};

#define ARRLEN(x) (sizeof(x) / sizeof((x)[0]))

int main() {
    int fd = open("test.txt", O_WRONLY);
    dup2(fd, STDOUT_FILENO);
    for (int i = 0; i < ARRLEN(strings); ++i) {
        if (!fork()) {
            puts(strings[i]);
            return 0;
        }
    }
    while (wait(NULL) > 0) {};
}
