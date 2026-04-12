#include <stdio.h>

int main(void)
{
    unsigned int a = 2147483647u;
    unsigned int b = 2147483647u;
    unsigned int result;
    result = a * b;
    printf("%u\n", result);
    return 0;
}