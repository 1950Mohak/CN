#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        exit(1);
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        close(sock);
        exit(1);
    }
    printf("Connected to server\n");

    // Chat loop
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        // Send message to server
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            close(sock);
            exit(1);
        }

        // Check for logout
        if (strcmp(buffer, "logout") == 0) {
            printf("Logged out. Connection closed.\n");
            break;
        }

        // Receive a reply from the server
        int recv_size = recv(sock, buffer, BUFFER_SIZE, 0);
        if (recv_size < 0) {
            perror("Recv failed");
            close(sock);
            exit(1);
        }
        buffer[recv_size] = '\0';  // Null-terminate the received string
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}
