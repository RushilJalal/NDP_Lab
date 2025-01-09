#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void handle_client(int client_fd)
{
    int buffer[BUFFER_SIZE];
    int n, choice;

    // receive the number of integers
    recv(client_fd, &n, sizeof(int), 0);

    // receive the integers
    recv(client_fd, buffer, n * sizeof(int), 0);
    while (1)
    {
        // receive the choice
        recv(client_fd, &choice, sizeof(int), 0);

        switch (choice)
        {
        case 1:
            // receive the number to be searched
            int num, found = 0;
            recv(client_fd, &num, sizeof(int), 0);

            // search for a number in the buffer
            for (int i = 0; i < n; i++)
            {
                if (buffer[i] == num)
                {
                    found = 1;
                    break;
                }
            }

            // send the result back to client
            send(client_fd, &found, sizeof(int), 0);
            break;

        case 2:
            // recv whether to sort in ascending or descending
            int order;
            recv(client_fd, &order, sizeof(int), 0);

            if (order == 1)
            {
                // sort in ascending order
                for (int i = 0; i < n - 1; i++)
                {
                    for (int j = 0; j < n - i - 1; j++)
                    {
                        if (buffer[j] > buffer[j + 1])
                        {
                            int temp = buffer[j];
                            buffer[j] = buffer[j + 1];
                            buffer[j + 1] = temp;
                        }
                    }
                }
            }
            else
            {
                // sort in descending order
                for (int i = 0; i < n - 1; i++)
                {
                    for (int j = 0; j < n - i - 1; j++)
                    {
                        if (buffer[j] < buffer[j + 1])
                        {
                            int temp = buffer[j];
                            buffer[j] = buffer[j + 1];
                            buffer[j + 1] = temp;
                        }
                    }
                }
            }

            // send back the results to server
            send(client_fd, buffer, n * sizeof(int), 0);
            break;

        case 3:
            // split the buffer elements into odd and even arrays
            int odd[BUFFER_SIZE], even[BUFFER_SIZE];
            int odd_count = 0, even_count = 0;

            for (int i = 0; i < n; i++)
            {
                if (buffer[i] % 2 == 0)
                {
                    even[even_count] = buffer[i];
                    even_count++;
                }
                else
                {
                    odd[odd_count] = buffer[i];
                    odd_count++;
                }
            }
            // send the odd, even buffers and odd,even counts
            send(client_fd, &even_count, sizeof(int), 0);
            send(client_fd, &odd_count, sizeof(int), 0);
            send(client_fd, even, even_count * sizeof(int), 0);
            send(client_fd, odd, odd_count * sizeof(int), 0);
            break;

        default:
            break;
        }
    }
    close(client_fd);
}

int main()
{
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        printf("Error in creating socket\n");
        return 1;
    }
    printf("Server started\n");

    // setup server details
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // binding - assigning a name to the socket
    // connects the server_fd to the server_addr
    int bind_return_val = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // casting required because different struct type(check)
    if (bind_return_val == -1)
    {
        printf("Error binding\n");
        close(server_fd);
        return 1;
    }

    // listening for connections on a socket
    int listen_return_val = listen(server_fd, 1);
    if (listen_return_val == -1)
    {
        printf("Error listening\n");
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
            printf("error in accepting client connection\n");
            close(server_fd);
            return 1;
        }
        handle_client(client_fd);
    }
    close(server_fd);
    return 0;
}