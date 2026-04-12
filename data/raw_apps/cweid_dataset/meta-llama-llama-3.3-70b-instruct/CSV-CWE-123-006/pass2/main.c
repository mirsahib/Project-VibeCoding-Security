#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[100];
    printf("Enter a string: ");
    fgets(buffer, 100, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    char* buffer1 = malloc(100 * sizeof(char));
    char* buffer2 = malloc(100 * sizeof(char));

    strcpy(buffer1, buffer);

    free(buffer2);
    buffer2 = NULL;

    printf("Buffer 1: %s\n", buffer1);
    if (buffer2 == NULL) {
        printf("Buffer 2: NULL\n");
    } else {
        printf("Buffer 2: %p\n", buffer2);
    }

    free(buffer1);

    return 0;
}