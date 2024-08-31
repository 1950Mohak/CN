#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Create client socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        exit(1);
    }

    printf("Connected to chat server...\n");

    while (1) {
        // Send message to server
        printf("Client: ");
        fgets(buffer, 256, stdin);
        send(client_fd, buffer, strlen(buffer), 0);

        // Receive response from server
        recv(client_fd, buffer, 256, 0);
        printf("Server: %s\n", buffer);

        // Check for logout message
        if (strcmp(buffer, "Server: Hello, client!") == 0) {
            printf("Disconnected from server...\n");
            break;
        }
    }

    close(client_fd);
    return 0;
}
