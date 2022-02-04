#include "opt.h"
#include <stdio.h>

int main() {
    printf("%d\n", optimized_away());
    uint32_t timestamps[100];
    moved_out(timestamps);
    printf("%x %x %x %x\n",
        timestamps[0], timestamps[1],
        timestamps[2], timestamps[3]);
}
