#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[256];
    printf("Enter input: ");
    if (fgets(buffer, 256, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        int length = 0;
        while (buffer[length]) {
            printf("%c", buffer[length]);
            length++;
        }
        printf("\n");
    }
    return 0;
}