#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");

    const wchar_t *src = L"Hello Wide World";
    size_t len = wcslen(src);
    wchar_t *dst = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));

    if (dst != NULL) {
        wcscpy(dst, src);
        wprintf(L"Original: %ls\n", src);
        wprintf(Copied: %ls\n", dst);
        free(dst);
    } else {
        wprintf(L"Memory allocation failed.\n");
    }
    return 0;
}