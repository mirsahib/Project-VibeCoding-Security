#include <stdio.h>

int main() {
    int a = 10;
    char b = 65;
    long c = 100000L;

    printf("Original int: %d\n", a);
    printf("Casting int to long: %ld\n", (long)a);

    printf("\nOriginal char: %d\n", b);
    printf("Casting char to int: %d\n", (int)b);

    printf("\nOriginal long: %ld\n", c);
    printf("Casting long to int (truncation): %d\n", (int)c);

    return 0;
}