#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_IP_LENGTH 16
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
    
    ip_address[strcspn(ip_address, "\n")] = '\0';

    int is_ipv4 = inet_pton(AF_INET, ip_address, &(struct in_addr){0}) == 1;
    int is_ipv6 = inet_pton(AF_INET6, ip_address, &(struct in6_addr){0}) == 1;

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