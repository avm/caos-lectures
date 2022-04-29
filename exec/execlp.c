#include <unistd.h>

int main(int argc, char* argv[]) {
    int status = execvp("echo", argv + 1);
    // error
}
