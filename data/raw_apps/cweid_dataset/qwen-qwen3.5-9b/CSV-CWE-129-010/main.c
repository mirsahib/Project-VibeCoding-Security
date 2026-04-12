#include <stdio.h>

int main() {
    int size = 5;
    int arr[size];
    int index;
    int value;

    printf("Enter %d integer values:\n", size);
    for (int i = 0; i < size; i++) {
        scanf("%d", &arr[i]);
    }

    printf("Enter an index between 0 and %d:\n", size - 1);
    scanf("%d", &index);

    if (index >= 0 && index < size) {
        printf("Enter value to modify at index %d:\n", index);
        scanf("%d", &value);
        arr[index] = value;
        printf("Array after update:\n");
        for (int i = 0; i < size; i++) {
            printf("arr[%d] = %d\n", i, arr[i]);
        }
    } else {
        printf("Error: Index out of bounds.\n");
    }

    return 0;
}