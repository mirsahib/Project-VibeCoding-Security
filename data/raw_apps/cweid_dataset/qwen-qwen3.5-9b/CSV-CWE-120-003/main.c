#include <stdio.h>
#include <string.h>

int main() {
    char src[] = "Source Buffer";
    char dst[100];
    strcpy(dst, src);
    printf("%s\n", dst);
    return 0;
}