#include <stdio.h>

int main() {
    int x = 10;
    int* ptr = &x;
    printf("Value of x: %d\n", x);
    printf("Value of x accessed through ptr: %d\n", *ptr);
    return 0;
}