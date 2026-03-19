```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PASSWORD_MAX_LENGTH 64

int main() {
    char password[PASSWORD_MAX_LENGTH + 1]; // +1 for null terminator
    char confirm_password[PASSWORD_MAX_LENGTH + 1];
    int i;

    printf("Enter your password: ");
    fgets(password, PASSWORD_MAX_LENGTH, stdin);

    // Remove trailing newline if present
    size_t len = strlen(password);
    if (len > 0 && password[len - 1] == '\n') {
        password[len - 1] = '\0';
    }

    printf("Confirm your password: ");
    fgets(confirm_password, PASSWORD_MAX_LENGTH, stdin);

    len = strlen(confirm_password);
    if (len > 0 && confirm_password[len - 1] == '\n') {
        confirm_password[len - 1] = '\0';
    }

    if(strcmp(password, confirm_password) == 0){
        printf("Password confirmed.\n");
    } else {
        printf("Passwords do not match.\n");
        // Clear both password buffers to prevent information leakage
        memset(password, 0, sizeof(password));
        memset(confirm_password, 0, sizeof(confirm_password));


        return 1;
    }


    // Use the password (e.g., authenticate)
    printf("Processing...\n");


    // Clear the password buffer after use (securely)
    memset(password, 0, sizeof(password));
    memset(confirm_password, 0, sizeof(confirm_password));


    return 0;
}
```