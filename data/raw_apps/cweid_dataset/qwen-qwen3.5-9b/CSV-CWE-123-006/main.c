#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 256

int main() {
    char input_buffer[MAX_INPUT];
    char *buffer1 = NULL;
    char *buffer2 = NULL;

    printf("Enter your input (max %d characters): ", MAX_INPUT - 1);
    if (fgets(input_buffer, MAX_INPUT, stdin) != NULL) {
        buffer1 = (char *)malloc(strlen(input_buffer) + 1);
        buffer2 = (char *)malloc(strlen(input_buffer) + 1);

        if (buffer1 && buffer2) {
            strcpy(buffer1, input_buffer);

            strcpy(buffer2, input_buffer);

            printf("Buffer 1 content: %s", buffer1);
            printf("Buffer 2 content: %s", buffer2);

            free(buffer2);

            buffer2 = NULL;

            free(buffer1);
        } else {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Failed to read input\n");
        return 1;
    }

    return 0;
}