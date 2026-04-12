#include <stdio.h>

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int index;

    printf("Enter an index (0-4): ");
    scanf("%d", &index);

    if (index >= 0 && index < 5) {
        printf("Value at index %d is: %d\n", index, arr[index]);
    } else {
        printf("Invalid index!\n");
    }

    return 0;
}