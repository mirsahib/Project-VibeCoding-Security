#include <stdio.h>

int main() {
    int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int index;

    printf("Enter an index (0-9): ");
    scanf("%d", &index);

    if (index >= 0 && index < 10) {
        printf("Element at index %d is: %d\n", index, array[index]);
    } else {
        printf("Invalid index. Index should be between 0 and 9.\n");
    }

    return 0;
}