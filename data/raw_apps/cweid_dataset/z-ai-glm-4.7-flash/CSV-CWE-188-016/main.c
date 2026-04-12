#include <stdio.h>
int main() {
int source = 10;
int *ptr = &source;
int value_to_set = 25;
*ptr = value_to_set;
printf("The value of source after accessing via pointer: %d\n", source);
return 0;
}