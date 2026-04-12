#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    char *ptr;
    int word_count = 0;

    printf("Enter some text: ");

    if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        printf("You entered: %s\n", buffer);

        ptr = buffer;
        int in_word = 0;

        while (*ptr != '\0') {
            if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
                in_word = 0;
            } else if (in_word == 0) {
                word_count++;
                in_word = 1;
            }
            ptr++;
        }

        printf("Word count: %d\n", word_count);
    } else {
        printf("Error reading input.\n");
    }

    return 0;
}