#include <stdio.h>
#include <stdint.h>

int main(void) {
    unsigned long long a = 4294967295ULL;
    unsigned long long b = 1ULL;
    unsigned long long sum = a + b;
    unsigned long long product = a * b;
    unsigned long long max32 = 4294967295ULL;

    printf("Max 32-bit Unsigned Value: %llu\n", max32);
    printf("Value a: %llu\n", a);
    printf("Value b: %llu\n", b);
    printf("Sum (4294967296): %llu\n", sum);
    printf("Product (4294967295): %llu\n", product);
    return 0;
}