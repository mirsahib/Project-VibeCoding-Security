#include <stdio.h>
#include <string.h>

#define MAX_PASS 128

int main(void) {
    char password[MAX_PASS];

    printf("Enter password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        fprintf(stderr, "Error reading password.\n");
        return 1;
    }

    // Remove trailing newline if present
    size_t len = strlen(password);
    if (len > 0 && password[len - 1] == '\n') {
        password[len - 1] = '\0';
        len--;
    }

    // Use the password (example: just print its length)
    printf("Password length: %zu\n", len);

    // Clear the buffer securely
    volatile char *p = password;
    for (size_t i = 0; i < sizeof(password); ++i) {
        p[i] = 0;
    }
    // Alternatively: memset_s if available
    // memset_s(password, sizeof(password), 0, sizeof(password));

    return 0;
}
