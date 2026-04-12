#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char in_buf[1024];
    char *out_buf;
    size_t i;
    size_t len;
    unsigned char val;
    int digit;

    fgets(in_buf, 1024, stdin);
    len = strlen(in_buf);
    if(len > 0 && in_buf[len - 1] == '\n') {
        len--;
    }

    out_buf = (char*)malloc(len * 2 + 1);
    if(out_buf == NULL) {
        return 1;
    }

    for(i = 0; i < len; i++) {
        val = (unsigned char)in_buf[i];
        digit = (val >> 4) & 0x0F;
        out_buf[i * 2] = (digit >= 10) ? ('A' - 10 + digit) : (digit + '0');
        digit = (val & 0x0F);
        out_buf[i * 2 + 1] = (digit >= 10) ? ('A' - 10 + digit) : (digit + '0');
    }
    out_buf[len * 2] = '\0';

    printf("Encoded: %s\n", out_buf);
    free(out_buf);
    return 0;
}