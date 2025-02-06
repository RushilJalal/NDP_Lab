#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wait.h>

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
    return (*(char *)a - *(char *)b);
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

int sendall(int sockfd, const void *buf, size_t len)
{
    size_t total = 0;       // how many bytes we've sent
    size_t bytesleft = len; // how many we have left to send
    int n;

    while (total < len)
    {
        n = send(sockfd, (char *)buf + total, bytesleft, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int main()
{
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket creation error");
        return 1;
    }

    // set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt error");
        close(sockfd);
        return 1;
    }

    // set server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6724);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    int bind_ret_val = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret_val == -1)
    {
        perror("binding error");
        close(sockfd);
        return 1;
    }

    printf("server up and running!\n");

    // listen for connections
    int listen_ret_val = listen(sockfd, 1);
    if (listen_ret_val == -1)
    {
        perror("unable to listen for connections error");
        close(sockfd);
        return 1;
    }

    // accept connections
    struct sockaddr_in client_addr;
    socklen_t actual_len = sizeof(client_addr);

    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &actual_len);
    if (clientfd == -1)
    {
        perror("error accepting connections");
        close(sockfd);
        return 1;
    }

    printf("client connected!\n");

    // recv string from client
    while (1)
    {
        char string[256];
        int n;
        if ((n = recv(clientfd, string, sizeof(string) - 1, 0)) == -1)
        {
            perror("error receiving string from client");
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
            if (sendall(clientfd, numbers, 256) == -1)
            {
                perror("error sending sorted numbers to client");
                close(clientfd);
                close(sockfd);
                return 1;
            }

            // send child process ID to client
            pid_t child_pid = getpid();
            char child_pid_str[256];
            printf("child PID: %d\n", child_pid);
            sprintf(child_pid_str, "%d", child_pid);
            if (sendall(clientfd, child_pid_str, 256) == -1)
            {
                perror("error sending child PID to client");
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
            if (sendall(clientfd, chars, 256) == -1)
            {
                perror("error sending sorted chars to client");
                close(clientfd);
                close(sockfd);
                return 1;
            }

            printf("number of bytes sent: %ld\n", strlen(chars) + 1);
            printf("Actual string bytes: %ld\n", sizeof(char) * strlen(chars));

            // send parent process ID to client
            pid_t parent_pid = getpid();
            printf("parent PID: %d\n", parent_pid);
            char parent_pid_str[256];
            sprintf(parent_pid_str, "%d", parent_pid);
            if (sendall(clientfd, parent_pid_str, 256) == -1)
            {
                perror("error sending parent PID to client");
                close(clientfd);
                close(sockfd);
                return 1;
            }

            // close(clientfd);
        }
    }

    close(sockfd);
    return 0;
}