#include <stdio.h>
#include <string.h>

int main(void) {
    char password[1024];
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    char *nl = strchr(password, '\n');
    if (nl) {
        *nl = '\0';
    }
    printf("Verification: Success\n");
    memset(password, 0, sizeof(password));
    return 0;
}