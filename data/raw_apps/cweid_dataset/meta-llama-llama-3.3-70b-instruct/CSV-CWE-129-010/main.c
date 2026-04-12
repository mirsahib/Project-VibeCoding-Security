#include <stdio.h>

int main() {
    int array[5] = {10, 20, 30, 40, 50};
    int index, value;

    printf("Enter the index of the array element you want to access (0-4): ");
    scanf("%d", &index);

    if(index >= 0 && index <= 4) {
        value = array[index];
        printf("The value at index %d is: %d\n", index, value);
    } else {
        printf("Invalid index. Index should be between 0 and 4.\n");
    }

    return 0;
}