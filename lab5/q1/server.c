// gcc server.c -o server -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

int client_count = 0;
pthread_mutex_t lock;

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Get client socket address
    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);

    // Receive data from client
    if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0)
    {
        printf("Received from client %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Append the received data to the file
        FILE *file = fopen("output.txt", "a");
        if (file == NULL)
        {
            perror("Failed to open file");
            close(client_socket);
            return NULL;
        }
        fprintf(file, "%s ", buffer);
        fclose(file);
    }

    // Close the client socket
    close(client_socket);

    // Decrement client count
    pthread_mutex_lock(&lock);
    client_count--;
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a new connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) == -1)
        {
            perror("Accept failed");
            continue;
        }

        // Check if the maximum number of clients is reached
        pthread_mutex_lock(&lock);
        if (client_count >= MAX_CLIENTS)
        {
            printf("Maximum clients reached. Terminating session.\n");
            send(client_socket, "terminate session", 18, 0);
            close(client_socket);
            pthread_mutex_unlock(&lock);
            break;
        }
        client_count++;
        pthread_mutex_unlock(&lock);

        // Create a new thread to handle the client
        if (pthread_create(&tid, NULL, handle_client, &client_socket) != 0)
        {
            perror("Thread creation failed");
            close(client_socket);
        }
        pthread_detach(tid);
    }

    // Close the server socket
    close(server_socket);

    // Read and display the file content
    FILE *file = fopen("output.txt", "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    char file_content[BUFFER_SIZE];
    fgets(file_content, BUFFER_SIZE, file);
    fclose(file);
    printf("Final content: %s\n", file_content);

    // Destroy mutex
    pthread_mutex_destroy(&lock);

    return 0;
}