#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

int main() {
    char ip_str[1024];
    char hostname[256];
    struct sockaddr_in addr;
    struct in_addr ip_addr;

    if (fgets(ip_str, sizeof(ip_str), stdin) == NULL) {
        return 1;
    }

    if (strlen(ip_str) > 0 && ip_str[strlen(ip_str) - 1] == '\n') {
        ip_str[strlen(ip_str) - 1] = '\0';
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    if (inet_pton(AF_INET, ip_str, &ip_addr) == 1) {
        addr.sin_addr = ip_addr;

        if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), hostname, sizeof(hostname), NULL, 0, NI_NOFQDN) == 0) {
            printf("%s\n", hostname);
        } else {
            printf("Hostname lookup failed\n");
        }
    } else {
        printf("Invalid IP address\n");
    }

    return 0;
}