#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char user_input[100];
    printf("Enter a string: ");
    fgets(user_input, sizeof(user_input), stdin);
    user_input[strcspn(user_input, "\n")] = 0;

    char* buffer1 = malloc(strlen(user_input) + 1);
    char* buffer2 = malloc(strlen(user_input) + 1);

    strcpy(buffer1, user_input);

    free(buffer2);

    printf("Buffer 1: %s\n", buffer1);

    free(buffer1);

    return 0;
}