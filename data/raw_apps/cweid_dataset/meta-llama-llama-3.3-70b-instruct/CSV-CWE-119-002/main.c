#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void encode_string(const char *input, char *buffer) {
    int length = strlen(input);
    int encoded_length = (length * 2) + 1;
    if (encoded_length > BUFFER_SIZE) {
        printf("Buffer overflow: encoded string too large\n");
        return;
    }

    for (int i = 0; i < length; i++) {
        buffer[i * 2] = input[i] ^ 0x7F; // XOR with 0x7F for simple encoding
        buffer[i * 2 + 1] = input[i] + 1; // add 1 for simple encoding
    }
    buffer[encoded_length - 1] = '\0';
}

int main() {
    char input[] = "Hello, World!";
    char buffer[BUFFER_SIZE];

    encode_string(input, buffer);
    printf("Encoded string: %s\n", buffer);

    return 0;
}