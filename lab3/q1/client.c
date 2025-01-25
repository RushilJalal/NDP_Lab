#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to server
    int connect_ret_val = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret_val == -1)
    {
        printf("Error connecting to server\n");
        close(sockfd);
        return 1;
    }

    // fork the process
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        printf("PPID of child: %d\n", getppid());
        printf("PID of child: %d\n", getpid());
    }
    else
    {
        printf("PPID of parent: %d\n", getppid());
        printf("PID of parent: %d\n", getpid());
    }

    while (1)
    {
        if (pid == 0)
        {
            // child process
            // read the message from console and send it to server

            char message[256];
            gets(message);

            // send message to server
            if (send(sockfd, message, sizeof(message), 0) == -1)
            {
                printf("error sending message from client to server\n");
                close(sockfd);
                return 1;
            }
        }
        else if (pid > 0)
        {
            // parent process
            // recv message from server and display it
            char message[256];
            if (recv(sockfd, message, sizeof(message), 0) == -1)
            {
                printf("error receiving message from server\n");
                close(sockfd);
                return 1;
            }

            // display message
            puts(message);
        }
    }

    close(sockfd);
    return 0;
}