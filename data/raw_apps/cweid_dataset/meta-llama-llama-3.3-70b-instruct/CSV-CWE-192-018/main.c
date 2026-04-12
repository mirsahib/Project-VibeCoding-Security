#include <stdio.h>

int main() {
    int a = 30000;
    unsigned char b = (unsigned char) a;
    char c = (char) a;
    short d = (short) a;
    long e = (long) a;
    long long f = (long long) a;
    unsigned int g = (unsigned int) a;
    unsigned short h = (unsigned short) a;
    unsigned long i = (unsigned long) a;
    unsigned long long j = (unsigned long long) a;

    printf("Original value: %d\n", a);
    printf("Unsigned char: %d\n", b);
    printf("Char: %d\n", c);
    printf("Short: %d\n", d);
    printf("Long: %ld\n", e);
    printf("Long long: %lld\n", f);
    printf("Unsigned int: %u\n", g);
    printf("Unsigned short: %u\n", h);
    printf("Unsigned long: %lu\n", i);
    printf("Unsigned long long: %llu\n", j);

    return 0;
}