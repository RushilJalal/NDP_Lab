#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

int main()
{
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation error\n");
        return 1;
    }

    // set server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    int bind_ret_val = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret_val == -1)
    {
        printf("binding error\n");
        close(sockfd);
        return 1;
    }

    printf("server up and running!\n");

    // listen for connections
    int listen_ret_val = listen(sockfd, 1);
    if (listen_ret_val == -1)
    {
        printf("unable to listen for connections error\n");
        close(sockfd);
        return 1;
    }

    // accept connections
    struct sockaddr_in client_addr;
    socklen_t actual_len = sizeof(client_addr);

    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &actual_len);
    if (clientfd == -1)
    {
        printf("error accepting connections\n");
        close(sockfd);
        return 1;
    }

    printf("client connected!\n");

    // fork the process
    pid_t pid;
    pid = fork();

    while (1)

    {
        if (pid == 0)
        {
            // child process
            // recv message from client and display it
            char message[256];
            if (recv(clientfd, message, sizeof(message), 0) == -1)
            {
                printf("error receiving message from client\n");
                close(clientfd);
                close(sockfd);
                return 1;
            }

            // if recvd message is "BYE", terminate
            if (strcasecmp(message, "BYE") == 0)
            {
                printf("Connection terminated!\n");
                close(clientfd);
                close(sockfd);
                return 1;
            }

            // display message
            puts(message);
        }
        else if (pid > 0)
        {
            // parent process
            // read message from console and send to client

            char message[256];
            gets(message);

            // send message to client
            if (send(clientfd, message, sizeof(message), 0) == -1)
            {
                printf("error in sending message to client\n");
                close(clientfd);
                close(sockfd);
                return 1;
            }
        }
    }

    close(clientfd);
    close(sockfd);
    return 0;
}