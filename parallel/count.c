#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>


struct data {
    int words[4096];
    _Atomic int ready_threads;
    _Atomic int go;
};

void child(struct data* data) {
    data->ready_threads++;
    while (!data->go) {
        asm volatile("pause");
    }
    for (int i = 0; i < 100000; i++) {
        asm ("incl %0" : "+m"(data->words[0]));
    }
    data->ready_threads--;
}

int main() {
    struct data* data = mmap(NULL, sizeof(struct data), PROT_READ | PROT_WRITE,
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    const int nthreads = 10;
    for (int i = 0; i < nthreads; ++i) {
        if (!fork()) {
            child(data);
            return 0;
        }
    }

    while (data->ready_threads < nthreads) {
        asm volatile("pause");
    }
    data->go = 1;
    while (data->ready_threads > 0) {
        asm volatile("pause");
    }
    printf("result: %d\n", data->words[0]);
    while (wait(NULL) > 0);
}
