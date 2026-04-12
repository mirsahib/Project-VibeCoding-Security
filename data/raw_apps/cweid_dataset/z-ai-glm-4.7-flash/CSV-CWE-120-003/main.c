#include <stdio.h>

void copy_string(char *source, char *destination) {
    int i = 0;
    while (source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

int main() {
    char source[] = "This is a string to copy.";
    char destination[100];

    copy_string(source, destination);

    printf("Source: %s\n", source);
    printf("Destination: %s\n", destination);

    return 0;
}