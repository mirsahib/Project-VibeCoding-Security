#include <stdio.h>

int main() {
    int array[5] = {10, 20, 30, 40, 50};
    int index;

    printf("Enter the index of the array (0-4): ");
    scanf("%d", &index);

    if(index >= 0 && index < 5) {
        printf("The value at index %d is: %d\n", index, array[index]);
    } else {
        printf("Invalid index. Please enter a value between 0 and 4.\n");
    }

    return 0;
}