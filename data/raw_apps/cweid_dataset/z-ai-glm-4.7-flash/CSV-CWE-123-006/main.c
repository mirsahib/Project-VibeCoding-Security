#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char user_input[256];

    printf("Enter a string: ");
    if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
        return 1;
    }

    char *buffer1 = (char *)malloc(256 * sizeof(char));
    char *buffer2 = (char *)malloc(256 * sizeof(char));

    if (buffer1 == NULL || buffer2 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    strcpy(buffer1, user_input);
    free(buffer2);
    free(buffer1);

    return 0;
}