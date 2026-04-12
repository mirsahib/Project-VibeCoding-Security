#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr;
    int n;

    printf("Enter the number of elements: ");
    scanf("%d", &n);

    ptr = (int*) malloc(n * sizeof(int));

    if(ptr == NULL) {
        printf("Memory not allocated.\n");
        return -1;
    }

    for(int i = 0; i < n; i++) {
        ptr[i] = i * 10;
        printf("Element %d: %d\n", i, ptr[i]);
    }

    free(ptr);
    return 0;
}