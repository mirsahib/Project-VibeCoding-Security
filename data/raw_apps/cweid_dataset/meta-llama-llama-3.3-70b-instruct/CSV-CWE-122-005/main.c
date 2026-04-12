#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* encode_string(const char* input) {
    int length = strlen(input);
    char* buffer = malloc((length * 2) + 1);
    if (buffer == NULL) {
        return NULL;
    }
    for (int i = 0; i < length; i++) {
        buffer[i * 2] = input[i];
        buffer[(i * 2) + 1] = input[i];
    }
    buffer[length * 2] = '\0';
    return buffer;
}

int main() {
    const char* input = "Hello, World!";
    char* encoded = encode_string(input);
    if (encoded != NULL) {
        printf("Encoded string: %s\n", encoded);
        free(encoded);
    } else {
        printf("Memory allocation failed\n");
    }
    return 0;
}