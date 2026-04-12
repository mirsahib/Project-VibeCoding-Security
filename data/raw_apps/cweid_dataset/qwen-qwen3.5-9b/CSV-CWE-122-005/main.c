#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* hex_encode(const char* src) {
    int len = strlen(src);
    char* dst = (char*)malloc(len * 2 + 1);
    if (!dst) return NULL;
    for (int i = 0; i < len; i++) {
        unsigned char byte = (unsigned char)src[i];
        int high = byte >> 4;
        int low = byte & 0x0F;
        if (high < 10) dst[i * 2 + 0] = high + '0';
        else           dst[i * 2 + 0] = (high - 10) + 'a';
        if (low < 10)  dst[i * 2 + 1] = low + '0';
        else           dst[i * 2 + 1] = (low - 10) + 'a';
    }
    dst[len * 2] = 0;
    return dst;
}

int main(void) {
    const char* input = "Hello World";
    char* encoded = hex_encode(input);
    if (encoded != NULL) {
        printf("%s\n", encoded);
        free(encoded);
    }
    return 0;
}