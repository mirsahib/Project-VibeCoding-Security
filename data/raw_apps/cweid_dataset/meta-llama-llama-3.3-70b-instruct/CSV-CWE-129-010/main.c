#include <stdio.h>

int main() {
    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);
    int arr[n];
    printf("Enter %d elements: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    printf("Enter the index of the element you want to access: ");
    int index;
    scanf("%d", &index);
    if (index >= 0 && index < n) {
        printf("Element at index %d is: %d\n", index, arr[index]);
    } else {
        printf("Invalid index\n");
    }
    return 0;
}