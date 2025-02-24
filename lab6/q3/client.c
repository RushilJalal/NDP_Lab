#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAXSIZE 50


void handle_comm(int sockfd) {
    int sentbytes;
    int recedbytes;
    char buff[MAXSIZE];

    while (1) {
        printf("Enter text to send to server: ");
        fgets(buff, sizeof(buff), stdin); // use fgets to allow spaces
        puts(buff);
        sentbytes = send(sockfd, buff, strlen(buff), 0);
        if (sentbytes == -1) {
            printf("Error in sending message\n");
            break;
        }
        memset(buff, 0, sizeof(buff));
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Error in receiving message\n");
            break;
        }
        if (recedbytes == 0) {
            break; // server closed the connection
        }
        printf("Corrected Code: ");
        puts(buff);

        sleep(2);

        memset(buff, 0, sizeof(buff));
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            printf("Error in receiving message\n");
            break;
        }
        if (recedbytes == 0) {
            break; // server closed the connection
        }
        printf("Odd Parity Value: ");
        puts(buff);
    }
    
}

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    // printf("%d", retval);
    if (retval == -1) {
        printf("Connection error\n");
        close(sockfd);
        return -1;
    }

    // if (fork() == 0) {
    //     receive_message(sockfd); // child process
    //     exit(0);
    // } else {
    //     send_message(sockfd); // parent process
    // }
    int i = 0;
    while(i < 1){
        handle_comm(sockfd);

    }

    close(sockfd);
    return 0;
}