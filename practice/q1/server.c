#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 256

typedef struct book
{
    int ID;
    char title[20];
    char publisher[20];
    char genre[20];
} book;

int main()
{
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Error in creating socket");
        return 1;
    }

    printf("Server started\n");

    // setup server details
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // binding - assigning a name to the socket
    // connects the server_fd to the server_addr
    int bind_return_val = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // casting required because different struct type(check)
    if (bind_return_val == -1)
    {
        perror("Error binding");
        close(server_fd);
        return 1;
    }

    // listening for connections on a socket
    int listen_return_val = listen(server_fd, 1);
    if (listen_return_val == -1)
    {
        perror("Error listening");
        close(server_fd);
        return 1;
    }

    printf("Server listening!\n");

    // accept connection from client
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1)
        {
            perror("error in accepting client connection");
            close(server_fd);
            return 1;
        }

        printf("Client connected\n");

        // recv choice of user
        int choice;
        recv(client_fd, &choice, sizeof(int), 0);

        book books[2];
        switch (choice)
        {
        case 1:
        {
            // insert info into books array and display on client side
            // recv books array
            if (recv(client_fd, books, sizeof(books), 0) == -1)
            {
                perror("Error receiving books array");
                close(client_fd);
                continue;
            }

            // display book info
            printf("Displaying book details: \n");
            for (int i = 0; i < 2; i++)
            {
                printf("Book ID: %d\n", books[i].ID);
                printf("Book Title: %s\n", books[i].title);
                printf("Genre: %s\n", books[i].genre);
                printf("Book Publisher: %s\n", books[i].publisher);
            }
            break;
        }

        case 2:
            // recv books array
            recv(client_fd, books, sizeof(books), 0);

            printf("Displaying book details: \n");
            for (int i = 0; i < 2; i++)
            {
                printf("Book ID: %d\n", books[i].ID);
                printf("Book Title: %s\n", books[i].title);
                printf("Genre: %s\n", books[i].genre);
                printf("Book Publisher: %s\n", books[i].publisher);
            }
            break;

            
        default:
            printf("Invalid choice\n");
            break;
        }
    }

    close(server_fd);
    return 0;
}
