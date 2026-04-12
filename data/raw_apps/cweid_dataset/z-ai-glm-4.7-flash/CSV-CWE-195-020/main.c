#include <stdio.h>
#include <limits.h>

int main() {
    int signed_var = -42;
    unsigned int unsigned_var = (unsigned int)signed_var;

    printf("Signed value: %d\n", signed_var);
    printf("Unsigned value: %u\n", unsigned_var);

    return 0;
}