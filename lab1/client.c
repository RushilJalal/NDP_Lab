#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#define MAXSIZE 50

int main()
{
    int sockfd, retval;
    int sentbytes;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    // Create client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address

    // Connect to the server
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    if (retval == -1) {
        printf("Connection error\n");
        return 1;
    }

    // Continuously prompt for user input and send messages to the server
    while (1) {
        printf("Enter message: ");
        scanf("%49s", buff);  // Prevent buffer overflow by limiting input size

        // Send the message to the server
        sentbytes = send(sockfd, buff, strlen(buff) + 1, 0);  // Send actual string length
        if (sentbytes == -1) {
            printf("Send error\n");
            close(sockfd);
            return 1;
        }
    }

    close(sockfd);
    return 0;
}
