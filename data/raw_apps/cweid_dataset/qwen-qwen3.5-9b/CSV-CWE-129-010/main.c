#include <stdio.h>

int main() {
    int arr[] = {100, 200, 300, 400, 500};
    int size = sizeof(arr) / sizeof(arr[0]);
    int index;

    printf("Enter an index from 0 to %d: ", size - 1);
    if (scanf("%d", &index) == 1) {
        if (index >= 0 && index < size) {
            printf("Value at index %d is %d\n", index, arr[index]);
        } else {
            printf("Index out of bounds\n");
        }
    } else {
        printf("Invalid input\n");
    }
    return 0;
}