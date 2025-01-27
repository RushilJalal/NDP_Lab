#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

void extract_numbers(char *string, char *result)
{
    int j = 0;
    for (int i = 0; string[i] != '\0'; i++)
    {
        if (isdigit(string[i]))
        {
            result[j++] = string[i];
        }
    }
    result[j] = '\0';
}

int compare_numbers(const void *a, const void *b)
{
    return (*(char *)a - '0') - (*(char *)b - '0');
}

void extract_chars(char *string, char *result)
{
    int j = 0;
    for (int i = 0; string[i] != '\0'; i++)
    {
        if (isalpha(string[i]))
        {
            result[j++] = string[i];
        }
    }
    result[j] = '\0';
}

int compare_chars(const void *a, const void *b)
{
    return (*(char *)b - *(char *)a);
}

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

    // recv string from client
    char string[256];
    int n;
    if ((n = recv(clientfd, string, sizeof(string) - 1, 0)) == -1)
    {
        printf("error receiving string from client\n");
        close(sockfd);
        close(clientfd);
        return 1;
    }

    // insert '\0' at the end of string
    string[n] = '\0';

    // fork the process
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process
        // extract the numbers and sort in ascending
        char numbers[256];
        extract_numbers(string, numbers);

        // sort the numbers
        qsort(numbers, strlen(numbers), sizeof(char), compare_numbers);

        // send numbers back to client
        if (send(clientfd, numbers, strlen(numbers) + 1, 0) == -1)
        {
            printf("error sending sorted numbers to client\n");
            close(clientfd);
            close(sockfd);
            return 1;
        }

        // send child process ID to client
        pid_t child_pid = getpid();
        if (send(clientfd, &child_pid, sizeof(child_pid), 0) == -1)
        {
            printf("error sending child PID to client\n");
            close(clientfd);
            close(sockfd);
            return 1;
        }

        close(clientfd);
        exit(0);
    }
    else if (pid > 0)
    {
        // parent process
        // wait for child process to finish
        wait(NULL);

        // extract the characters and sort in reverse
        char chars[256];
        extract_chars(string, chars);

        // sort the chars in reverse
        qsort(chars, strlen(chars), sizeof(char), compare_chars);

        printf("sorted chars: %s\n", chars);

        // send sorted chars back to client
        int n;
        if ((n = send(clientfd, chars, strlen(chars) + 1, 0)) == -1)
        {
            printf("error sending sorted chars to client\n");
            close(clientfd);
            close(sockfd);
            return 1;
        }

        printf("number of bytes sent: %d\n", n);
        printf("Actual string bytes: %ld\n", sizeof(char) * strlen(chars));

        // send parent process ID to client
        pid_t parent_pid = getpid();
        if (send(clientfd, &parent_pid, sizeof(parent_pid), 0) == -1)
        {
            printf("error sending parent PID to client\n");
            close(clientfd);
            close(sockfd);
            return 1;
        }

        close(clientfd);
    }

    close(sockfd);
    return 0;
}