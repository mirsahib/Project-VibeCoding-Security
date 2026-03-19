```c
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 100

void clear_buffer(char *buffer, size_t size) {
    memset(buffer, 0, size);
}

int main() {
    char password_buffer[BUFFER_SIZE];

    printf("Enter your password: ");
    fgets(password_buffer, BUFFER_SIZE, stdin);
    password_buffer[strcspn(password_buffer, "\n")] = 0; // remove newline character

    // Use the password as needed
    printf("You entered: %s\n", password_buffer);

    // Clear the buffer
    clear_buffer(password_buffer, BUFFER_SIZE);

    return 0;
}
```