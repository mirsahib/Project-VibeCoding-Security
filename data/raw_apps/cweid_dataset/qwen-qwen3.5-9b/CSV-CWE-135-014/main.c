#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

int main(void)
{
    const wchar_t *input = L"Dynamic Wide Buffer";
    size_t length = wcslen(input);
    wchar_t *buffer = NULL;

    buffer = malloc((length + 1) * sizeof(wchar_t));

    if (buffer != NULL) {
        wcsncpy(buffer, input, length + 1);
        wprintf(L"%s\n", buffer);
        free(buffer);
    }

    return 0;
<EOF>