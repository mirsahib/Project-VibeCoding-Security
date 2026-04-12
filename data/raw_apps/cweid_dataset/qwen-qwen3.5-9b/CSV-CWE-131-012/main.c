#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr;
    int i;
    int size = 5;

    ptr = (int *)malloc(size * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (i = 0; i < size; i++) {
        ptr[i] = i * 10;
    }

    for (i = 0; i < size; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");

    free(ptr);
    return 0;
}