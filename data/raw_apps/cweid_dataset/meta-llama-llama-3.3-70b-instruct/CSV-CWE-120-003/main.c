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
    char src[] = "Hello, World!";
    char dest[20];

    copy_string(src, dest);

    printf("Original string: %s\n", src);
    printf("Copied string: %s\n", dest);

    return 0;
}