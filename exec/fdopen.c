#include <unistd.h>
#include <stdio.h>

int main() {
    int fds0[2];
    int fds1[2];
    if (pipe(fds0)) {
        perror("pipe");
        return 1;
    }
    if (pipe(fds1)) {
        perror("pipe");
        return 1;
    }

    if (!fork()) {
        // child
        int i; 
        close(fds0[1]);
        FILE* pipereader = fdopen(fds0[0], "rt");
        while (fscanf(pipereader, "%d", &i) == 1) {
            i *= i;
            write(fds1[1], &i, sizeof(i));
        }
        return 0;
    }

    FILE* pipewriter = fdopen(fds0[1], "wt");
    for (int i = 0; i < 10; ++i) {
        fprintf(pipewriter, "%d\n", i);
        fflush(pipewriter);
        int j;
        read(fds1[0], &j, sizeof(j));
        printf("parent received %d\n", j);
    }
}
