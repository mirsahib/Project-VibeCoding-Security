#include <stdio.h>
int main() {
    int i_val = 100;
    short s_val = 50;
    long l_val = 10000;
    unsigned int u_val = 200;
    int i_from_short = (int)s_val;
    int i_from_long = (int)l_val;
    short s_from_int = (short)i_val;
    int u_to_int = (int)u_val;
    long u_to_long_val = (long)u_val;
    printf("int from short: %d\n", i_from_short);
    printf("int from long: %d\n", i_from_long);
    printf("short from int: %d\n", s_from_int);
    printf("int from unsigned int: %d\n", u_to_int);
    printf("long from unsigned int: %ld\n", u_to_long_val);
    return 0;
}