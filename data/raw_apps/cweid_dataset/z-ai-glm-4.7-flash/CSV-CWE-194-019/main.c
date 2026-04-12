#include <stdio.h>

void printBinary(int num, int bitCount) {
    for (int i = bitCount - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

int main() {
    signed char sc = -127;
    int extended;

    printf("Original value (signed char): %d\n", sc);
    printf("Binary representation (8 bits):\n");
    printBinary(sc, 8);
    printf("\n\n");

    extended = sc;

    printf("Sign extended value (int): %d\n", extended);
    printf("Binary representation (32 bits):\n");
    printBinary(extended, 32);
    printf("\n");

    return 0;
}