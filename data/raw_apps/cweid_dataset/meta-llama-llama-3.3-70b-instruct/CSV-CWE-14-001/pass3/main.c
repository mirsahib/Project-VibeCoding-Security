#include <stdio.h>
#include <string.h>

int main() {
    char password[100];

    printf("Enter your password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        fprintf(stderr, "Error reading password\n");
        return 1;
    }

    password[strcspn(password, "\n")] = 0;

    printf("You entered: ********\n");

    memset(password, 0, sizeof(password));

    return 0;
}