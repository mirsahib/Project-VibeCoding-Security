#include <stdio.h>
#include <string.h>

void encode_string_to_buffer(const char *input, char *output_buffer, size_t buffer_capacity) {
    if (input == NULL || output_buffer == NULL) return;

    size_t input_length = strlen(input);

    for (size_t i = 0; i < input_length; i++) {
        // Simple encoding: bitwise XOR with a constant key
        output_buffer[i] = input[i] ^ 0x55;
    }

    if (input_length < buffer_capacity) {
        output_buffer[input_length] = '\0';
    }
}

int main() {
    char original_string[] = "Hello, World!";
    char encoded_storage[64];

    encode_string_to_buffer(original_string, encoded_storage, sizeof(encoded_storage));

    printf("Original: %s\nEncoded:   %s\n", original_string, encoded_storage);

    return 0;
}