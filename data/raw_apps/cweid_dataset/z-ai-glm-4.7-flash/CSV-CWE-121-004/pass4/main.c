#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_IP_LENGTH 50
#define MAX_HOSTNAME_LENGTH 256

int main() {
    char ip_address[MAX_IP_LENGTH];
    char hostname[MAX_HOSTNAME_LENGTH];
    struct addrinfo hints, *res, *p;
    int status;
    void *addrptr;

    printf("Enter IP address: ");
    if (fgets(ip_address, sizeof(ip_address), stdin) == NULL) {
        return 1;
    }
    
    // Remove trailing newline and carriage return safely
    // Check for buffer overflow: if fgets read the whole buffer, there is no newline
    for (int i = 0; ip_address[i]; i++) {
        if (ip_address[i] == '\n') {
            ip_address[i] = '\0';
            break;
        }
    }
    // If the input was longer than the buffer, the newline is missing.
    // Check if we consumed the whole buffer to determine if input was too long.
    // (Though fgets already null-terminated, we need to ensure we don't have buffer overrun in validation)

    // Validate IPv4
    struct in_addr ipv4test;
    int is_ipv4 = inet_pton(AF_INET, ip_address, &ipv4test) == 1;
    
    // Validate IPv6 using standard library
    struct in6_addr ip6addr;
    int is_ipv6 = inet_pton(AF_INET6, ip_address, &ip6addr) == 1;

    if (!is_ipv4 && !is_ipv6) {
        printf("Error: Invalid IP address format.\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(ip_address, NULL, &hints, &res);
    if (status != 0) {
        printf("Error: %s\n", gai_strerror(status));
        return 1;
    }

    char ipstr[INET6_ADDRSTRLEN];
    struct sockaddr_storage *sa;

    for (p = res; p != NULL; p = p->ai_next) {
        sa = (struct sockaddr_storage *)p->ai_addr;
        if (sa->ss_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;
            addrptr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)sa;
            addrptr = &(ipv6->sin6_addr);
        }

        status = getnameinfo(addrptr, (sa->ss_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), hostname, sizeof(hostname), NULL, 0, 0);
        if (status == 0) {
            break;
        }
    }

    if (status != 0) {
        printf("Error: Unable to resolve hostname.\n");
    } else {
        printf("Hostname: %s\n", hostname);
    }

    freeaddrinfo(res);
    return 0;
}