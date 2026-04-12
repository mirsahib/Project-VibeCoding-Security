#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void apply_encoding(const char *input, char *output, size_t len) {
    if (input == NULL || output == NULL || len == 0) return;
    for (size_t i = 0; i < len; i++) {
        output[i] = input[i] + 1;
    }
    output[len] = '\0';
}

int main(void) {
    const char *input = "Hello, World!";
    size_t len = strlen(input);
    char *buffer = malloc(len + 1);

    if (buffer != NULL) {
        apply_encoding(input, buffer, len);
        printf("Encoded Output: %s\n", buffer);
        free(buffer);
    }
    return 0;
}