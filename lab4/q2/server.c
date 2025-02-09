#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "database.txt"

// Function to find IP address for a given domain
char *lookup_ip(char *domain)
{
    static char ip[BUFFER_SIZE]; // Static buffer to store the IP address
    char file_domain[BUFFER_SIZE], file_ip[BUFFER_SIZE];
    FILE *file = fopen(DB_FILE, "r");

    if (!file)
    {
        perror("Error opening database file");
        return "ERROR: Database not found";
    }

    while (fscanf(file, "%s %s", file_domain, file_ip) != EOF)
    {
        if (strcmp(domain, file_domain) == 0)
        {
            fclose(file);
            strncpy(ip, file_ip, BUFFER_SIZE); // Copy result to static buffer
            return ip;
        }
    }

    fclose(file);
    return "ERROR: Domain not found";
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Bind the socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server is running on port %d...\n", PORT);

    while (1)
    {
        // Accept a connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Read client request
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Client requested: %s\n", buffer);

        // Lookup IP and send response
        char *response = lookup_ip(buffer);
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    return 0;
}
