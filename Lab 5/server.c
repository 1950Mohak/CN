#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 10

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[256];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Bind server socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("binding failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listening failed");
        exit(1);
    }

    printf("Chat server started on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accepting failed");
            continue;
        }

        // Create a new process to handle the client
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // Child process handles the client
            close(server_fd);
            printf("Connected to client IP address %s and port %d...\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            while (1) {
                // Receive message from client
                recv(client_fd, buffer, 256, 0);
                printf("Client: %s\n", buffer);

                // Check for logout message
                if (strcmp(buffer, "logout") == 0) {
                    printf("Client disconnected...\n");
                    break;
                }

                // Send response back to client
                send(client_fd, "Server: Hello, client!", 20, 0);
            }

            close(client_fd);
            exit(0);
        } else {
            // Parent process closes client socket
            close(client_fd);
        }
    }

    return 0;
}

