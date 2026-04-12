#include <stdio.h>
#include <string.h>

int main() {
    char source[100] = "Hello";
    char destination[100];

    strcpy(destination, source);

    printf("%s\n", destination);

    return 0;
}