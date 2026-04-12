#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

void process_input(char *buffer) {
    printf("You entered: %s\n", buffer);
}

int main() {
    char input_buffer[MAX_SIZE];

    printf("Enter a string: ");
    fgets(input_buffer, MAX_SIZE, stdin);

    input_buffer[strcspn(input_buffer, "\n")] = 0;

    process_input(input_buffer);

    return 0;
}