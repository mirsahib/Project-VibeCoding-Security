#include <stdio.h>
#include <string.h>

int main(void)
{
    char password[100];
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    int i;
    i = (int)strlen(password);
    if (i > 0 && password[i - 1] == '\n') {
        password[i - 1] = '\0';
    }
    printf("Password cleared after usage.\n");
    memset(password, 0, sizeof(password));
    return 0;
}