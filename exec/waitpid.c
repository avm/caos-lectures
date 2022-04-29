#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

/*

        ....
        mov SYS_fork, %eax
        int $0x80
eip->   ...   // %eax == 0

        ....
        mov SYS_fork, %eax int $0x80
eip        ...   // %eax == child_pid


*/

int main() {
    pid_t children[5];
    for (int i = 0; i < 5; ++i) {
        if ((children[i] = fork()) == 0) {
            printf("child %d\n", getpid());
            sleep(5-i);
            return i;
        }
    }
    pid_t child;
    for (int i = 0; i < 50; ++i) {
        child = waitpid(-1, NULL, WNOHANG);
        if (child > 0) {
            printf("reaped %d\n", child);
        } else {
            sleep(1);
        }
    }
}
