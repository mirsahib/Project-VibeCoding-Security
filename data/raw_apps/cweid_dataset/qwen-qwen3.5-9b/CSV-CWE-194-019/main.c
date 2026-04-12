#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    uint8_t small = 255;
    uint32_t result;
    int8_t signed_val = -42;
    int32_t extended_val;
    int16_t value;
    uint32_t mask = 0xFF;
    int sign;

    printf("=== Numeric Operations and Sign Extension Demo ===\n\n");

    // Original unsigned value
    printf("Original uint8_t: 0x%02X (%d)\n", small, small);

    // Perform sign extension from 8-bit to 32-bit
    signed_val = (char)small - 1;
    printf("Converted signed (8-bit): %d (char)\n", signed_val);

    // Sign extend to 32-bit
    extended_val = ((int32_t)signed_val << 24) >> 24;
    printf("Sign extended to 32-bit: %d\n", extended_val);

    // Direct sign extension using cast
    int16_t partial = (int16_t)(char)0xFF & 0xFFFF;
    printf("Partial 16-bit extend: %d\n", partial);

    // Method 2: Using comparison
    if (signed_val & 0x80) {
        extended_val = -1;  // Sign bit set
    } else {
        extended_val = signed_val;
    }
    printf("Sign extension test: %d\n", extended_val);

    // Another numeric operation example
    unsigned char a = 4;
    unsigned char b = 7;
    unsigned char c = a * b;
    printf("Multiplication: %d * %d = %d\n", a, b, c);

    // Bitwise operations with sign extension
    int8_t neg = -16;
    uint8_t abs_neg = neg & 0xFF;
    printf("\nBitwise analysis:\n");
    printf("int8_t: %d (binary: %08b)\n", neg, neg & 0xFF);
    printf("unsigned 0xFF mask: %u\n", abs_neg);

    // Shift operations
    int16_t shifted = signed_val;
    shifted = (shifted << 4) >> 4;
    printf("After shift round trip: %d\n", shifted);

    // Additional arithmetic
    int64_t total = (int64_t)(signed_val * extended_val);
    printf("\n64-bit calculation: %d\n", total);

    // Flag variable demonstration
    flag = 0;
    if (small == 255) {
        flag = 1;
    }
    printf("Flag set: %d\n", flag);

    return 0;
}