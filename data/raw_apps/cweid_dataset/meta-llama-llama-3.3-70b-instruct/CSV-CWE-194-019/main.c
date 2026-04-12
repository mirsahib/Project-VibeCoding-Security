#include <stdio.h>
#include <stdint.h>

int main() {
    int8_t byte = -10;
    int16_t short_int = (int16_t)byte;
    int32_t int_ext = (int32_t)byte;

    printf("Original signed byte: %d\n", byte);
    printf("Sign-extended to short int: %d\n", short_int);
    printf("Sign-extended to int: %d\n", int_ext);

    return 0;
}