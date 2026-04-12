#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(void)
{
    char ip_string[16];
    char hostname[256];
    struct in_addr addr;

    printf("Enter IP address: ");
    if (scanf("%15s", ip_string) != 1)
    {
        return 1;
    }

    if (inet_pton(AF_INET, ip_string, &addr) != 1)
    {
        printf("Invalid IP address format.\n");
        return 1;
    }

    struct hostent *host = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    if (host == NULL)
    {
        printf("Lookup failed: %s\n", hstrerror(h_errno));
        return 1;
    }

    strcpy(hostname, host->h_name);
    printf("Hostname: %s\n", hostname);

    return 0;
}