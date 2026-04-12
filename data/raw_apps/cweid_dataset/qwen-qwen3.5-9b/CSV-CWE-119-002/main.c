#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void apply_encoding(const char *input, char *buffer) {
    size_t len = strlen(input);
    for (size_t i = 0; i < len; i++) {
        buffer[i * 2] = "0123456789ABCDEF"[input[i] >> 4];
        buffer[i * 2 + 1] = "0123456789ABCDEF"[input[i] & 0xF];
    }
    buffer[len * 2] = '\0';
}

int main() {
    const char *source_text = "DataString";
    size_t len = strlen(source_text);
    char output_buffer[len * 2 + 1];
    apply_encoding(source_text, output_buffer);
    return 0;
}