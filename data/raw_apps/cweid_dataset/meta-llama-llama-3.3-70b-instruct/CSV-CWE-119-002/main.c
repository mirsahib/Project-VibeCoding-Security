#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void encode_string(const char *input, char *buffer) {
    int length = strlen(input);
    for (int i = 0; i < length; i++) {
        buffer[i * 2] = input[i];
        buffer[i * 2 + 1] = input[i];
    }
    buffer[length * 2] = '\0';
}

int main() {
    char input[100];
    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    char buffer[BUFFER_SIZE];
    encode_string(input, buffer);
    printf("Encoded string: %s\n", buffer);

    return 0;
}