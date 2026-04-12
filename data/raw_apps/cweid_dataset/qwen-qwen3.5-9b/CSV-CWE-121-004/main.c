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
    int i;

    printf("Enter IP address: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }

    if (!inet_aton(input, &addr)) {
        fprintf(stderr, "Invalid IP address\n");
        return 1;
    }

    host = gethostbyaddr(&addr.s_addr, sizeof(struct in_addr), AF_INET);
    if (host == NULL) {
        fprintf(stderr, "No hostname found\n");
        return 1;
    }

    strcpy(buffer, host->h_name);
    printf("Hostname: %s\n", buffer);
    return 0;
}