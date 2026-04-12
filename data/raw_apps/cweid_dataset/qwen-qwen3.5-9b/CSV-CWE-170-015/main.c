#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    char buffer[4096];
    int n;

    fp = fopen("cfgfile", "r");
    if (fp == NULL) {
        return 1;
    }
    n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    if (n > 0) {
        buffer[n] = '\0';
    }
    fclose(fp);
    return 0;
}