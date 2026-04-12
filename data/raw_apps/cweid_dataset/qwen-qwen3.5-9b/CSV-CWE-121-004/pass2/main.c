#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    char input[256];
    char buffer[256];
    struct in_addr addr;
    struct hostent *host;
    size_t i;

    printf("Enter IP address: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }

    // Remove newline character left by fgets
    input[strcspn(input, "\n")] = '\0';

    if (!inet_aton(input, &addr)) {
        fprintf(stderr, "Invalid IP address\n");
        return 1;
    }

    host = gethostbyaddr(&addr.s_addr, sizeof(struct in_addr), AF_INET);
    if (host == NULL) {
        fprintf(stderr, "No hostname found\n");
        return 1;
    }

    // Fix Snyk Vulnerability: Use strncpy instead of strcpy to prevent buffer overflow
    if (host->h_name && host->h_addr_list[0]) {
        strncpy(buffer, host->h_name, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        // Security check: If hostname is suspiciously long, we truncate.
        // We proceed to print regardless as the vulnerability is mitigated by strncpy.
    }

    printf("Hostname: %s\n", buffer);
    return 0;
}