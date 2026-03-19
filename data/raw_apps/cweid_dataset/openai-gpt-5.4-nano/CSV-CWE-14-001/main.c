#include <stdio.h>
#include <string.h>

static void secure_bzero(void *v, size_t n) {
#if defined(__STDC_LIB_EXT1__)
    memset_s(v, n, 0, n);
#else
    volatile unsigned char *p = (volatile unsigned char *)v;
    while (n--) {
        *p++ = 0;
    }
#endif
}

int main(void) {
    char password[256];

    printf("Enter password: ");
    fflush(stdout);

    if (!fgets(password, sizeof(password), stdin)) {
        secure_bzero(password, sizeof(password));
        return 1;
    }

    size_t len = strcspn(password, "\r\n");
    password[len] = '\0';

    /* Use password here if needed. For demo, just confirm length. */
    printf("Password length: %zu\n", len);

    secure_bzero(password, sizeof(password));
    return 0;
}