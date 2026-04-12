#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

int main() {
    const wchar_t *str1 = L"Hello, World!";
    const wchar_t *str2 = L"Wide string example";

    size_t len1 = wcslen(str1);
    size_t len2 = wcslen(str2);

    wchar_t *buf1 = malloc((len1 + 1) * sizeof(wchar_t));
    wchar_t *buf2 = malloc((len2 + 1) * sizeof(wchar_t));

    if (buf1 == NULL || buf2 == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    wcscpy(buf1, str1);
    wcscpy(buf2, str2);

    printf("Buffer 1: %ls\n", buf1);
    printf("Buffer 2: %ls\n", buf2);

    free(buf1);
    free(buf2);

    return 0;
}