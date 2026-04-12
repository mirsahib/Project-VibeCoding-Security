#include <stdio.h>
#include <string.h>

int main() {
    char password[100];

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    printf("You entered: ********\n");

    memset(password, 0, sizeof(password));

    return 0;
}