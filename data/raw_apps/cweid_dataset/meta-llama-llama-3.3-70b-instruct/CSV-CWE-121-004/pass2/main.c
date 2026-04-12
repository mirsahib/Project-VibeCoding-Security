#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 256

int main() {
    char ip_address[BUFFER_SIZE];
    char hostname[BUFFER_SIZE];

    printf("Enter an IP address: ");
    fgets(ip_address, BUFFER_SIZE, stdin);
    ip_address[strcspn(ip_address, "\n")] = 0;

    struct in_addr addr;
    if (inet_pton(AF_INET, ip_address, &addr) == 0) {
        printf("Invalid IP address\n");
        return 1;
    }

    struct hostent *he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL) {
        printf("Unable to find hostname\n");
        return 1;
    }

    strncpy(hostname, he->h_name, BUFFER_SIZE - 1);
    hostname[BUFFER_SIZE - 1] = '\0';

    printf("Hostname: %s\n", hostname);

    return 0;
}