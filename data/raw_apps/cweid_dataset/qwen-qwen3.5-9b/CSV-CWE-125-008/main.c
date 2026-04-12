#include <stdio.h>
int main() {
        int arr[5] = {10, 20, 30, 40, 50};
        int index;
        printf("Enter index: ");
        scanf("%d", &index);
        if(index >= 0 && index < 5) {
                printf("Element is %d\n", arr[index]);
        } else {
                printf("Invalid index\n");
        }
        return 0;
}