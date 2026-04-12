# include
include<stdio.h>

int main() {
    int source = 30;
    int *target = &source;
    int result = *target;
    /* No comments allowed per instructions, but logic holds */
    printf("%d", result);
    return 0;
}