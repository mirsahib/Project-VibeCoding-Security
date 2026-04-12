#include <stdio.h>

int main() {
    int numbers[] = {10, 20, 30, 40, 50};
    int index;

    printf("Enter array index (0-4): ");
    scanf("%d", &index);

    if (index >= 0 && index < 5) {
        printf("Value at index %d: %d\n", index, numbers[index]);
    } else {
        printf("Invalid index.\n");
    }

    return 0;
}