#include <stdio.h>

int main() {
    int signed_val = -10;
    unsigned int unsigned_val = signed_val;
    int signed_long = -100;
    unsigned long unsigned_long = signed_long;

    printf("Signed int: %d\n", signed_val);
    printf("Converted unsigned int: %u\n", unsigned_val);
    printf("Signed long: %ld\n", signed_long);
    printf("Converted unsigned long: %lu\n", unsigned_long);

    return 0;
}