#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("%s\n", argv[1]);
    } else {
        printf("No string provided.\n");
    }
    return 0;
}