#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[read_size] = '\0';  // Null-terminate the received string
        printf("Client: %s\n", buffer);

        // Check for "logout" message
        if (strcmp(buffer, "logout") == 0) {
            printf("Client requested to logout. Terminating connection...\n");
            break;
        }

        // Echo the message back to the client
        if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            close(client_socket);
            exit(1);
        }
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("Recv failed");
    }

    close(client_socket);
    exit(0);
}

void sigchld_handler(int s) {
    // Wait for all dead processes
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_socket, client_socket, c;
    struct sockaddr_in server, client;
    struct sigaction sa;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        exit(1);
    }
    printf("Socket created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }
    printf("Bind done\n");

    // Listen for incoming connections
    listen(server_socket, 3);

    // Handle zombie processes
    sa.sa_handler = sigchld_handler; // Reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Connection accepted\n");

        // Fork a new process to handle the client
        if (fork() == 0) {
            // Child process
            close(server_socket); // Close listening socket in child
            handle_client(client_socket);
        } else {
            // Parent process
            close(client_socket); // Close client socket in parent
        }
    }

    close(server_socket);
    return 0;
}
