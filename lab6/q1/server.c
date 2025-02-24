#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#define MAXSIZE 90

int even_parity_check(char buff[MAXSIZE]){
    int i = 0;
    int count = 0;
    for(i=0; buff[i] != '\0'; i++){
        if(buff[i] == '1')
            count++;
    }
    if(count % 2 == 0){
        printf("No Corruption!\n");
        return 0;
    }
    else
        return 1;
}

int odd_parity_check(char buff[MAXSIZE]){
    int i = 0;
    int count = 0;
    for(i=0; buff[i] != '\0'; i++){
        if(buff[i] == '1')
            count++;
    }
    if(count % 2 == 0)
        return 1;
    else{
        printf("No Corruption!\n");
        return 0;
    }
}

void handle_client(int newsockfd)
{
    int recedbytes, sentbytes;
    char buff[MAXSIZE];
    while (1)
    {
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1)
        {
            printf("Error in receiving message\n");
            break;
        }
        if (recedbytes == 0)
        {
            break;
        }
        printf("Text Received from Client: ");
        puts(buff);

        int check = even_parity_check(buff);
        
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%d", check);

        printf("Even Parity Value being sent: ");
        puts(buff);
        sentbytes = send(newsockfd, buff, strlen(buff), 0);
        if (sentbytes == -1)
        {
            printf("Error in sending message\n");
            break;
        }

        check = odd_parity_check(buff);
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%d", check);

        printf("Odd Parity Value being sent: ");
        puts(buff);
        sentbytes = send(newsockfd, buff, strlen(buff), 0);
        if (sentbytes == -1)
        {
            printf("Error in sending message\n");
            break;
        }
    }
    close(newsockfd);
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == 1)
    {
        perror("Binding error");
        close(sockfd);
        return -1;
    }

    retval = listen(sockfd, 1);
    printf("Server is listening on port 3388...\n");
    if (retval == -1)
    {
        perror("Listen error");
        close(sockfd);
        return -1;
    }

    actuallen = sizeof(clientaddr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        printf("%s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        if (newsockfd == -1)
        {
            printf("Accept error\n");
            close(sockfd);
            return -1;
        }

        handle_client(newsockfd);
    }

    close(sockfd);
    return 0;
}