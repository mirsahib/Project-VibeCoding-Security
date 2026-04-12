#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>

typedef struct {
    uint32_t length;
    char body[4096];
} ReceivedMessage;

void parse_and_extract(sockfd, char destString[]) {
    ReceivedMessage packet;
    char headerBuff[4];
    int receivedBytes;
    
    receivedBytes = recv(sockfd, headerBuff, 4, MSG_WAITALL);
    if (receivedBytes != 4) {
        return;
    }
    
    packet.length = *(uint32_t*)headerBuff;
    packet.length = ntohl(packet.length);
    
    receivedBytes = recv(sockfd, packet.body, packet.length, MSG_WAITALL);
    if (receivedBytes != packet.length) {
        packet.body[0] = '\0';
        destString[0] = '\0';
        return;
    }
    
    packet.body[receivedBytes] = '\0';
    
    strncpy(destString, packet.body, 4096);
    destString[4096] = '\0';
}

int main() {
    int sockfd;
    char localCopy[4096];
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return 1;
    }
    
    // Initialize socket connection details here
    
    parse_and_extract(sockfd, localCopy);
    
    printf("Extracted message: %s\n", localCopy);
    
    close(sockfd);
    return 0;
}