#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 256

int main() {
    char input_buffer[BUFFER_SIZE];
    char processed_buffer[BUFFER_SIZE];

    printf("Enter text to be processed:\n");

    // Reading input into the buffer
    if (fgets(input_buffer, BUFFER_SIZE, stdin) != NULL) {
        size_t len = strlen(input_buffer);

        // Remove trailing newline if present
        if (len > 0 && input_buffer[len - 1] == '\n') {
            input_buffer[len - 1] = '\0';
            len--;
        }

        // Process buffer: Convert to uppercase
        size_t i;
        for (i = 0; i < len; i++) {
            if (input_buffer[i] >= 'a' && input_buffer[i] <= 'z') {
                processed_buffer[i] = input_buffer[i] - 32;
            } else {
                processed_buffer[i] = input_buffer[i];
            }
        }
        processed_buffer[i] = '\0';

        printf("Original: %s\n", input_buffer);
        printf("Processed (Upper): %s\n", processed_buffer);
    } else {
        printf("Error reading input or empty buffer.\n");
    }

    return 0;
}