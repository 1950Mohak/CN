#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Sender IP> <Receiver Port>\n", argv[0]);
        return -1;
    }

    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in receiver_addr, sender_addr;
    int n;
    socklen_t len;

    // Creating socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&receiver_addr, 0, sizeof(receiver_addr));
    memset(&sender_addr, 0, sizeof(sender_addr));

    // Filling receiver information
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(atoi(argv[2]));
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the receiver address
    if (bind(sockfd, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    len = sizeof(sender_addr);

    while (1) {
        // Receive message from sender
        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&sender_addr, &len);
        buffer[n] = '\0';
        printf("Sender: %s\n", buffer);

        // Check if exit message was received
        if (strcmp(buffer, "exit") == 0) {
            printf("Sender has exited. Closing connection...\n");
            break;
        }

        // Input message to send
        printf("You: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

        // Send response
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&sender_addr, len);

        // Check if exit message was sent
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
