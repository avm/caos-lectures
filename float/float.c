#include <stdio.h>
#include <math.h>

int main() {
    float f;

    scanf("%f", &f);

    f = sinf(f) / 2.0f;

    printf("%f\n", f);

    return 0;
}
