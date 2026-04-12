#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char buffer[256];
    printf("Enter text to process: ");
    size_t len = fgets(buffer, sizeof(buffer), stdin) != NULL ? strlen(buffer) : 0;
    printf("Buffer received. Length: %zu\n", len);
    for (size_t i = 0; i < len && buffer[i] != '\n'; i++) {
        buffer[i] = toupper((unsigned char)buffer[i]);
    }
    printf("Processed output: %s\n", buffer);
    return 0;
}