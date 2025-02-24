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

// P1 P2 D3 P4 D5 D6 D7
// C1 = EVEN_PARITY(1, 3, 5, 7)
// C2 = EVEN_PARITY(2, 3, 6, 7)
// C4 = EVEN_PARITY(4, 5, 6, 7)

int even_parity_check(char buff[MAXSIZE]){
    int i = 0;
    int count = 0;
    for(i=0; buff[i] != '\0'; i++){
        if(buff[i] == '1')
            count++;
    }
    if(count % 2 == 0){
        // printf("No Corruption!\n");
        return 0;
    }
    else
        return 1;
}

int hamming(char buff[MAXSIZE]){ // 1100010
 
    if (MAXSIZE < 7) {
        return -1;  
    }

    char c1[5], c2[5], c4[5];
    c1[0] = buff[0]; // 1
    c1[1] = buff[2]; // 0
    c1[2] = buff[4]; // 0
    c1[3] = buff[6]; // 0
    c1[4] = '\0';

    c2[0] = buff[1]; // 1
    c2[1] = buff[2]; // 0
    c2[2] = buff[5]; // 1
    c2[3] = buff[6]; // 0
    c2[4] = '\0';

    c4[0] = buff[3]; // 0
    c4[1] = buff[4]; // 0
    c4[2] = buff[5]; // 1
    c4[3] = buff[6]; // 0
    c4[4] = '\0';

    printf("c1, c2, c4 values: \n");
    puts(c1); // 1
    puts(c2); // 0
    puts(c4); // 1

    int check1, check2, check4;
    check1 = even_parity_check(c1);
    check2 = even_parity_check(c2);
    check4 = even_parity_check(c4);

    printf("check1, check2, check4 values: %d, %d, %d\n", check1, check2, check4);

    int error_bit = 4*check4 + 2*check2 + check1;

    if(buff[error_bit - 1] == '0'){
        buff[error_bit - 1] = '1';
    }else{
        buff[error_bit - 1] = '0';
    }
    return error_bit;
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

        int check = hamming(buff);
        

        printf("Error in bit no. %d!\n", check);
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