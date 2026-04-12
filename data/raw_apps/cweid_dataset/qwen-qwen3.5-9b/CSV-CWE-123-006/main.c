#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char input_buffer[256];
    char* buffer1 = malloc(256);
    char* buffer2 = malloc(256);
    
    if (buffer1 == NULL || buffer2 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    printf("Enter text: ");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    
    strcpy(buffer1, input_buffer);
    
    free(buffer2);
    
    printf("Buffer 1: %s\n", buffer1);
    printf("Buffer 2: Freed\n");
    
    free(buffer1);
    
    return 0;
}