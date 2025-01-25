#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// udp

int main()
{
    // create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation error\n");
        return 1;
    }

    // set server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_addr_len = sizeof(server_addr);

    // input string from user
    char string[256];
    printf("Enter string: ");
    gets(string);

    // send string to server
    if (sendto(sockfd, string, sizeof(string), 0, (struct sockaddr *)&server_addr, server_addr_len) == -1)
    {
        printf("error sending string to server\n");
        close(sockfd);
        return 1;
    }

    // recv permutation string from server
    char result[8192];
    if (recvfrom(sockfd, result, sizeof(result), 0, (struct sockaddr *)&server_addr, &server_addr_len) == -1)
    {
        printf("error receiving permutation result from server\n");
        close(sockfd);
        return 1;
    }

    printf("Result: \n");
    puts(result);

    close(sockfd);
    return 0;
}