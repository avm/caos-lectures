#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

enum { NTHREADS = 10000000000 };

struct thr {
    pthread_t id;
    int i;
};

void* func(void* arg) {
    printf("arg: %p\n", arg);
    sleep(10);
}

int main() {
    for (int i = 0; 1; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 16 << 10);
        int err; 
        pthread_t thr;
        if (err = pthread_create(&thr, &attr, func, NULL)) {
            printf("failed with i == %d\n", i);
            sleep(1);
            return 0;
        }
    }
}
