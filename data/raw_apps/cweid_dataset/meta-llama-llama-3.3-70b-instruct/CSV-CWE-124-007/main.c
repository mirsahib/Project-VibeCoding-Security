#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void process_input(char *buffer) {
    printf("You entered: %s\n", buffer);
}

int main() {
    char buffer[BUFFER_SIZE];

    printf("Enter a line of text: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character

    process_input(buffer);

    return 0;
}