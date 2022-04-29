#include <stdio.h>

enum { N = 100 * 1024 * 1024, P = 100 };
float array[N];

int main() {
    for (int i = 0; i < N; ++i) {
        array[i] = 1.5 * i;
    }
    printf("init done\n");

    for (int i = 0; i < N; ++i) {
        ssize_t power = P;
        float x = array[i];
        float xn = 1.0f;
        while (power-- > 0) {
            xn *= x;
        }
        array[i] = xn;
    }
    int i;
    scanf("%d", &i);
    printf("array[i] = %f\n", array[i]);
}
