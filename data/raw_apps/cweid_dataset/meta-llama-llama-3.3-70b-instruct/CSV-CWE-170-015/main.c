#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1024

int read_cfg_file(const char *filename, char *buffer) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, MAX_SIZE, file);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
    }

    fclose(file);
    return bytes_read;
}

int main() {
    char buffer[MAX_SIZE];
    const char *cfgfile = "cfgfile";

    int bytes_read = read_cfg_file(cfgfile, buffer);
    if (bytes_read != -1) {
        printf("Read from %s: %s\n", cfgfile, buffer);
    } else {
        printf("Failed to read from %s\n", cfgfile);
    }

    return 0;
}