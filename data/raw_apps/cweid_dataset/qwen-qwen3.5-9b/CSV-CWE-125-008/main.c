#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10

int main() {
    int arr[ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int index;
    
    printf("Array elements: ");
    for(int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    printf("Enter array index (0-%d): ", ARRAY_SIZE - 1);
    int contador = scanf("%d", &index);
    
    if(contador != 1 || index < 0 || index >= ARRAY_SIZE) {
        printf("Invalid input. Index must be between 0 and %d\n", ARRAY_SIZE - 1);
        return 1;
    }
    
    printf("The value at index %d is: %d\n", index, arr[index]);
    
    return 0;
}