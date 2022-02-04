#include <stdio.h>
#include <syscall.h>

int main() {
    int a = 5, b = 7;
    int c;

    asm ( "add %2, %1\n\t"
          "mov %1, %0"
          : "=m"(c), "+r"(a)// outputs
          : "g"(7)  // inputs
          : "esi" ); // clobbers

    printf("%d\n", c);

    int retval;
    asm ( "int $0x80"
          : "=a"(retval)
          : "a"(SYS_exit), "b"(42) );
}
