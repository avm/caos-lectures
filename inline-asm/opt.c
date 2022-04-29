#include <stdint.h>
#include <stddef.h>

int optimized_away() {
    volatile char string[] = "a very long string\n";
    volatile char *p = string;
    // Old school strlen
    // scasb: compare %al with (%edi) and set flags, then inc %edi
    // rep: do { } while(%ecx-- !=0 && !zf)
    // repnz: repeat while not ZF
    //        (dec %ecx)
    asm ( "repnz scasb" : "+D"(p) : "a"(0), "c"(1024) : "cc" );
    return p - string;
}

int moved_out(uint32_t timestamps[100]) {
    for (size_t i = 0; i < 100; ++i) {
        uint32_t ts;
        asm volatile ( "rdtsc" : "=a"(ts) : : "edx" );
        timestamps[i] = ts;
    }
}
