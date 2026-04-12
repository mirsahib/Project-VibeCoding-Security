#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int message_length;
    char body[1024];
} MessageData;

void process_data(char* data) {
    printf("Processing received data: %s\n", data);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(1);
    }

    int length;
    ssize_t valread = recv(sockfd, &length, sizeof(int), 0);
    if (valread <= 0) {
        perror("Failed to read length");
        close(sockfd);
        exit(1);
    }

    char body[1024] = {0};
    valread = recv(sockfd, body, length, 0);
    if (valread <= 0) {
        perror("Failed to read body");
        close(sockfd);
        exit(1);
    }

    MessageData message;
    message.message_length = length;
    strcpy(message.body, body);

    char local_string[1024] = {0};
    strcpy(local_string, message.body);

    process_data(local_string);

    close(sockfd);
    return 0;
}