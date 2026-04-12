#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_MESSAGE_LENGTH 1024

typedef struct {
    int length;
    char body[MAX_MESSAGE_LENGTH];
} message_t;

void processMessage(char* msg) {
    printf("Received message: %s\n", msg);
    // Add further processing here
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char buffer[BUFFER_SIZE];
    message_t msg;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Set server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // Listen for connections
    if (listen(sockfd, 3) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server listening on port 8080...\n");

    // Accept connection
    connfd = accept(sockfd, (struct sockaddr *)&client, &client_len);
    if (connfd < 0) {
        perror("accept failed");
        exit(1);
    }

    printf("Connection accepted from client IP address %s and port %d...\n",
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // Receive message from client
    len = recv(connfd, &msg, sizeof(msg), 0);
    if (len < 0) {
        perror("recv failed");
        exit(1);
    }

    // Copy message body into local character string
    char local_msg[msg.length + 1];
    strncpy(local_msg, msg.body, msg.length);
    local_msg[msg.length] = '\0';

    // Process message
    processMessage(local_msg);

    // Close sockets
    close(connfd);
    close(sockfd);

    return 0;
}