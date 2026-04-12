#include <stdio.h>
#include <string.h>

int main() {
    char password[100];
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    printf("Password received.\n");
    memset(password, 0, sizeof(password));
    return 0;
}