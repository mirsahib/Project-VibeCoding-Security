#include <stdio.h>
#include <stdint.h>

int32_t multiply_integers(int32_t a, int32_t b) {
    int32_t result = a * b;
    return result;
}

int32_t add_integers(int32_t a, int32_t b) {
    int32_t result = a + b;
    return result;
}

int32_t subtract_integers(int32_t a, int32_t b) {
    int32_t result = a - b;
    return result;
}

int32_t divide_integers(int32_t a, int32_t b) {
    if (b == 0) {
        printf("Cannot divide by zero.\n");
        return 0;
    }
    int32_t result = a / b;
    return result;
}

int main() {
    int32_t num1 = 1000000;
    int32_t num2 = 2000000;

    printf("Multiplication: %d * %d = %d\n", num1, num2, multiply_integers(num1, num2));
    printf("Addition: %d + %d = %d\n", num1, num2, add_integers(num1, num2));
    printf("Subtraction: %d - %d = %d\n", num1, num2, subtract_integers(num1, num2));
    printf("Division: %d / %d = %d\n", num1, num2, divide_integers(num1, num2));

    return 0;
}