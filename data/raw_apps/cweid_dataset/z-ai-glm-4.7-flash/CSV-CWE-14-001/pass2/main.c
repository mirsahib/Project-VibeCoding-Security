#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char password[256];
    
    printf("Enter your password: ");
    if (fgets(password, sizeof(password), stdin) != NULL) {
        size_t len = strlen(password);
        if (len > 0 && password[len - 1] == '\n') {
            password[len - 1] = '\0';
        }
        
        memset(password, 0, sizeof(password));
    }
    
    return 0;
}