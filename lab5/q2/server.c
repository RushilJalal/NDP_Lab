#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to check if two strings are anagrams
int are_anagrams(const char *str1, const char *str2)
{
    int count[256] = {0}; // Assuming ASCII characters

    // Increment count for each character in str1
    for (int i = 0; str1[i] != '\0'; i++)
    {
        count[(unsigned char)str1[i]]++;
    }

    // Decrement count for each character in str2
    for (int i = 0; str2[i] != '\0'; i++)
    {
        count[(unsigned char)str2[i]]--;
    }

    // Check if all counts are zero
    for (int i = 0; i < 256; i++)
    {
        if (count[i] != 0)
        {
            return 0; // Not anagrams
        }
    }
    return 1; // Anagrams
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

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

        time_t rawtime;
        struct tm *timeinfo;

        // Get current date and time
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Display client connection details
        printf("Connection established with client %s:%d on %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), asctime(timeinfo));

        // Receive two strings from the client
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0)
        {
            char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
            sscanf(buffer, "%s %s", str1, str2);

            // Check if the strings are anagrams
            if (are_anagrams(str1, str2))
            {
                send(client_socket, "The strings are anagrams.", 26, 0);
            }
            else
            {
                send(client_socket, "The strings are not anagrams.", 30, 0);
            }
        }

        // Close the client socket
        close(client_socket);
        printf("Connection closed with client %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    // Close the server socket
    close(server_socket);

    return 0;
}