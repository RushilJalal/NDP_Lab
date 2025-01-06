#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 90

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recvdbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    // Create server socket
    // will store 3 since 0,1,2 are reserved
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("\nSocket creation error\n");
        return 1;
    }

    printf("Server listening...\n");

    // Set up server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server socket to the address
    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Binding error\n");
        close(sockfd);
        return 1;
    }

    // Listen for incoming connections
    retval = listen(sockfd, 1);
    if (retval == -1)
    {
        printf("Listen error\n");
        close(sockfd);
        return 1;
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

    if (newsockfd == -1)
    {
        printf("Accept error\n");
        close(sockfd);
        return 1;
    }

    printf("Client connected!\n");

    // Continuously receive and display messages from client
    while (1)
    {
        recvdbytes = recv(newsockfd, buff, sizeof(buff) - 1, 0); // Receive up to the buffer size
        if (recvdbytes == -1)
        {
            printf("Receive error\n");
            break;
        }

        buff[recvdbytes] = '\0'; // Null-terminate the received string
        printf("Received from client: %s\n", buff);
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
