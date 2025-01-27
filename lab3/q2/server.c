#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

// udp

void swap(char *x, char *y)
{
    char temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *a, int l, int r, char *result)
{
    //*a - pointer to the string
    // use pointer arithmetic to access different chars at different pos
    // base case - we have complete permutation
    if (l == r)
    {
        strcat(result, a);
        strcat(result, "\n");
    }
    else
    {

        // recursive case - generate permutations by swapping chars
        for (int i = l; i <= r; i++)
        {
            swap(a + l, a + i);           // swap current char with char at pos i
            permute(a, l + 1, r, result); // recursively generate permutations
            swap(a + l, a + i);           // undo the swap to get original string
        }
    }
}

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
    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t client_addr_len = sizeof(client_addr);

    // bind socket
    int bind_ret_val = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret_val == -1)
    {
        printf("binding error\n");
        close(sockfd);
        return 1;
    }

    printf("UDP server up and running!\n");

    // recv string from client
    char string[256];
    int n = recvfrom(sockfd, string, sizeof(string), 0, (struct sockaddr *)&client_addr, &client_addr_len);

    if (n == -1)
    {
        printf("error receiving string from user\n");
        close(sockfd);
        return 1;
    }

    // string is not null terminated by default when using send, sendto
    string[n] = '\0';

    printf("string received from client!\n");

    // obtain all permutations of the string
    char result[8192];
    permute(string, 0, strlen(string) - 1, result);

    printf("result: %s", result);

    // send back the permutation result back to client
    if (sendto(sockfd, result, sizeof(result), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
    {
        printf("error sending permutations to client\n");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}