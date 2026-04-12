#include <stdio.h>
#include <string.h>

void copy_string(char *src, char *dest) {
    while (*src) {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';
}

int main() {
    char src[100] = "Hello, World!";
    char dest[100];

    copy_string(src, dest);
    printf("Copied string: %s\n", dest);

    return 0;
}