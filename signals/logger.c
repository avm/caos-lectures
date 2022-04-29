#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

sig_atomic_t rotate = 0;

void handle(int sig) {
    rotate = 1;
}

int main(int argc, char* argv[]) {
    char buf[1024];
    int ofile = open("log.txt", O_WRONLY | O_APPEND | O_CREAT, S_IREAD | S_IWRITE);

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle;

    sigaction(SIGHUP, &sa, NULL);

    while (1) {
        int result = read(STDIN_FILENO, buf, sizeof(buf));
        if (result == 0) {
            return 0;
        }
        if (rotate) {
            close(ofile);
            ofile = open("log.txt", O_WRONLY | O_APPEND | O_CREAT, S_IREAD | S_IWRITE);
            rotate = 0;
        }
        if (result > 0) {
            write(ofile, buf, result);
        }
    }

}
