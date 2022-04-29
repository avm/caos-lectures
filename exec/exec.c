#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t child;
    if ((child = fork()) == 0) {
        int fd = open("test.txt", O_CREAT | O_WRONLY | O_APPEND, S_IREAD | S_IWRITE);
        printf("opened %d\n", fd);
        dup2(fd, STDOUT_FILENO);
        int status = execvp("ls", argv);
        return 127;
    }
    if (child < -1) {
        perror("fork");
    }
    int status;
    waitpid(child, &status, 0);
    printf("status %d\n", (status));
}
