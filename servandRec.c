// Server code (server.c)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_ARRAY_SIZE 1000

int find_second_largest(int arr[], int size) {
    int largest = arr[0], second_largest = arr[0];
    
    for(int i = 1; i < size; i++) {
        if(arr[i] > largest) {
            second_largest = largest;
            largest = arr[i];
        }
        else if(arr[i] > second_largest && arr[i] < largest) {
            second_largest = arr[i];
        }
    }
    return second_largest;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    // Receive array size
    int size;
    read(new_socket, &size, sizeof(size));
    printf("Receiving array of size: %d\n", size);
    
    // Receive array elements
    int arr[MAX_ARRAY_SIZE];
    for(int i = 0; i < size; i++) {
        read(new_socket, &arr[i], sizeof(int));
    }
    
    // Display received array
    printf("Received array: ");
    for(int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    // Find and display second largest element
    int second_largest = find_second_largest(arr, size);
    printf("Second largest element: %d\n", second_largest);
    
    close(new_socket);
    close(server_fd);
    return 0;
}

// Client code (client.c)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    // Get array size from user
    int size;
    printf("Enter the size of array: ");
    scanf("%d", &size);
    
    // Send array size to server
    send(sock, &size, sizeof(size), 0);
    
    // Get array elements from user and send to server
    int num;
    printf("Enter %d array elements:\n", size);
    for(int i = 0; i < size; i++) {
        scanf("%d", &num);
        send(sock, &num, sizeof(num), 0);
    }
    
    close(sock);
    return 0;
}
