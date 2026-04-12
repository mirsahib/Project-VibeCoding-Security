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
    int sockfd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    if (listen(sockfd, 3) < 0) {
        perror("Listen");
        exit(1);
    }
    if ((new_socket = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(1);
    }

    int length;
    ssize_t valread = recv(new_socket, &length, sizeof(int), 0);
    if (valread <= 0) {
        perror("Failed to read length");
        close(new_socket);
        close(sockfd);
        exit(1);
    }

    char body[1024] = {0};

    if (length <= 0) {
        perror("Invalid length");
        close(new_socket);
        close(sockfd);
        exit(1);
    }

    size_t bytes_to_read = (size_t)length;
    if (bytes_to_read > 1024) {
        bytes_to_read = 1024;
    }

    valread = recv(new_socket, body, bytes_to_read, 0);
    if (valread <= 0) {
        perror("Failed to read body");
        close(new_socket);
        close(sockfd);
        exit(1);
    }

    MessageData message;
    message.message_length = length;
    strncpy(message.body, body, sizeof(message.body) - 1);
    message.body[sizeof(message.body) - 1] = '\0';

    char local_string[1024] = {0};
    strncpy(local_string, message.body, sizeof(local_string) - 1);
    local_string[sizeof(local_string) - 1] = '\0';

    process_data(local_string);

    close(new_socket);
    close(sockfd);
    return 0;
}