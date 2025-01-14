#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>

// UDP
int main()
{
    // create client socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        printf("error creating client socket\n");
        return 1;
    }

    // fill server address details
    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    socklen_t server_addr_len = sizeof(server_addr);

    while (1)
    { // receive string from client and store in buffer
        char buffer[256];
        printf("Enter string:\n");
        // fgets(buffer, sizeof(buffer), stdin);
        gets(buffer);

        // send the string to the server
        if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            printf("error sending string to server\n");
            close(sockfd);
            return 1;
        }

        printf("Message sent to server\n");

        // receive result from server whether palindrome or not
        int isPalindrome;
        if (recvfrom(sockfd, &isPalindrome, sizeof(int), 0, (struct sockaddr *)&server_addr, &server_addr_len) == -1)
        {
            printf("error in receiving result from client\n");
            close(sockfd);
            return 1;
        }

        // receive length of string
        int string_len;
        if (recvfrom(sockfd, &string_len, sizeof(int), 0, (struct sockaddr *)&server_addr, &server_addr_len) == -1)
        {
            printf("error in receiving result length from client\n");
            close(sockfd);
            return 1;
        }

        // receive occurences of vowels
        int vowels[5];
        if (recvfrom(sockfd, vowels, 5 * sizeof(int), 0, (struct sockaddr *)&server_addr, &server_addr_len) == -1)
        {
            printf("error in receiving vowel occurences from client\n");
            close(sockfd);
            return 1;
        }

        // display results
        if (isPalindrome == 1)
        {
            printf("String is a palindrome\n");
        }
        else
        {
            printf("String is NOT a palindrome\n");
        }

        printf("Length of string: %d\n", string_len);

        printf("Occurences of vowels: \n");
        printf("a: %d\n", vowels[0]);
        printf("e: %d\n", vowels[1]);
        printf("i: %d\n", vowels[2]);
        printf("o: %d\n", vowels[3]);
        printf("u: %d\n", vowels[4]);
    }

    close(sockfd);
}