#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_IP_LENGTH 50
#define MAX_HOSTNAME_LENGTH 256

int is_valid_ipv4(const char *ip) {
    if (ip == NULL || strlen(ip) == 0) return 0;
    if (strlen(ip) >= MAX_IP_LENGTH) return 0;

    int segments = 0;
    const char *start = ip;
    const char *end = ip;
    long num = 0;

    while (*end) {
        if (*end == '.') {
            segments++;
            // Check bounds and if start points to empty
            if (start == end) return 0;

            char seg_str[end - start + 1];
            strncpy(seg_str, start, end - start);
            seg_str[end - start] = '\0';

            // Check if all characters are digits
            int digit_only = 1;
            for (int i = 0; seg_str[i]; i++) {
                if (seg_str[i] < '0' || seg_str[i] > '9') {
                    digit_only = 0;
                    break;
                }
            }
            if (!digit_only) return 0;

            // Convert to long to check range
            num = strtol(seg_str, NULL, 10);
            if (num < 0 || num > 255) return 0;

            start = end + 1;
        }
        end++;
    }
    
    // Check the last segment
    if (start == end) return 0;

    char seg_str[end - start + 1];
    strncpy(seg_str, start, end - start);
    seg_str[end - start] = '\0';

    int digit_only = 1;
    for (int i = 0; seg_str[i]; i++) {
        if (seg_str[i] < '0' || seg_str[i] > '9') {
            digit_only = 0;
            break;
        }
    }
    if (!digit_only) return 0;

    num = strtol(seg_str, NULL, 10);
    if (num < 0 || num > 255) return 0;

    return segments == 3;
}

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
    
    // Remove trailing newline and carriage return
    ip_address[strcspn(ip_address, "\n")] = '\0';
    ip_address[strcspn(ip_address, "\r")] = '\0';

    // Validate IPv4
    int is_ipv4 = is_valid_ipv4(ip_address);
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