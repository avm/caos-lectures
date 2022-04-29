
    3 * 8;
    exit(42);
    printf("...");

    if ((c = getchar()) != EOF) ...
    a = b = 5;

    x = foo() + bar();

    printf("%d %d", i++, i++);

    // CSE
    int array[100];
    array[i] = array[j];
    // DCE

    __attribute__((pure))
    int strlen(const char *s) {
        ...
    }


    // __attribute__((const))
    static int sum_squares(int x, int y) {
        return x*x + y*y;
    }

    float hypot(int x, int y) {
        return sqrt(sum_squares());
    }


    size_t autoincrement() {
        static hash_table ...
    }
    
    #include <string.h>
    size_t strlen(const char *);
    size_t autoincrement(const char *);

    int main() {
        strlen
    }
