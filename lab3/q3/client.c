#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

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

    // input string from user
    char buffer[256];
    printf("Enter alphanumeric string: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

    // send alphanumeric string to server
    if (send(sockfd, buffer, strlen(buffer) + 1, 0) == -1)
    {
        printf("error sending string to server\n");
        close(sockfd);
        return 1;
    }

    // receive sorted numbers from server
    char numbers[256];
    if (recv(sockfd, numbers, sizeof(numbers), 0) == -1)
    {
        printf("error receiving sorted numbers from server\n");
        close(sockfd);
        return 1;
    }

    // receive child process ID from server
    pid_t child_pid;
    if (recv(sockfd, &child_pid, sizeof(child_pid), 0) == -1)
    {
        printf("error receiving child PID from server\n");
        close(sockfd);
        return 1;
    }

    sleep(1);

    // receive sorted chars from server
    char chars[256];
    int size;
    if ((size = recv(sockfd, chars, sizeof(chars), 0)) == -1)
    {
        printf("error receiving sorted chars from server\n");
        close(sockfd);
        return 1;
    }

    sleep(1);

    printf("Number of bytes received by client: %d\n", size);

    // receive parent process ID from server
    pid_t parent_pid;
    if (recv(sockfd, &parent_pid, sizeof(parent_pid) - 4, 0) == -1)
    {
        printf("error receiving parent PID from server\n");
        close(sockfd);
        return 1;
    }

    // display results
    printf("Child Process ID: %d\n", child_pid);
    printf("Sorted Numbers: %s\n", numbers);
    printf("Parent Process ID: %d\n", parent_pid);
    printf("Sorted Characters: %s\n", chars);

    close(sockfd);
    return 0;
}