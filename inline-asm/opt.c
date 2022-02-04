#include <stdint.h>
#include <stddef.h>

int optimized_away() {
    char string[] = "a very long string\n";
    char *p = string;
    asm( "repnz scasb" : "+D"(p) : "a"(0), "c"(1024) );
    return p - string;
}

int moved_out(uint32_t timestamps[100]) {
    for (size_t i = 0; i < 100; ++i) {
        uint32_t ts;
        asm( "rdtsc" : "=a"(ts) : : "edx" );
        timestamps[i] = ts;
    }
}
