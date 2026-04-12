#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char hex_chars[] = "0123456789ABCDEF";

void encode_string(const char *src, char *dst) {
    int len = strlen(src);
    int i = 0;
    int j = 0;
    while (i < len) {
        dst[j] = hex_chars[(src[i] >> 4) & 0xf];
        j++;
        dst[j] = hex_chars[src[i] & 0xf];
        j++;
        i++;
    }
    dst[j] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    char input[256];
    strncpy(input, argv[1], sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    unsigned int len = strlen(input);
    unsigned int size = len * 2 + 1;
    char *buffer = (char *)malloc(size);

    if (buffer == NULL) {
        return 1;
    }

    encode_string(input, buffer);
    printf("%s\n", buffer);
    free(buffer);
    return 0;
}