#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    int8_t signed_char = -5;
    uint8_t unsigned_char = (uint8_t) signed_char;
    printf("Signed char: %" PRId8 "\n", signed_char);
    printf("Unsigned char: %" PRIu8 "\n", unsigned_char);

    int16_t signed_short = -10;
    uint16_t unsigned_short = (uint16_t) signed_short;
    printf("Signed short: %" PRId16 "\n", signed_short);
    printf("Unsigned short: %" PRIu16 "\n", unsigned_short);

    int32_t signed_int = -20;
    uint32_t unsigned_int = (uint32_t) signed_int;
    printf("Signed int: %" PRId32 "\n", signed_int);
    printf("Unsigned int: %" PRIu32 "\n", unsigned_int);

    int64_t signed_long = -30;
    uint64_t unsigned_long = (uint64_t) signed_long;
    printf("Signed long: %" PRId64 "\n", signed_long);
    printf("Unsigned long: %" PRIu64 "\n", unsigned_long);

    return 0;
}