#include <sys/socket.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>

// UDP
int main()
{
    // create socket
    int serverfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverfd == -1)
    {
        printf("Error creating socket\n");
        return 1;
    }
    // setup server details
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    int bind_value = bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_value == -1)
    {
        printf("Error binding the socket\n");
        close(serverfd);
        return 1;
    }

    printf("Server up and running!\n");

    while (1)
    { // recv a string from the client and store in buffer
        char buffer[256];
        struct sockaddr_in client_addr;

        int client_addr_len = sizeof(client_addr);
        int buffer_len = recvfrom(serverfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        int string_length = strlen(buffer);
        printf("String length: %d\n", string_length);

        if (buffer_len == -1)
        {
            printf("Error receiving bytes from client\n");
            close(serverfd);
            return 1;
        }

        // check if the received string is a palindrome
        int isPalindrome = 1;
        for (int i = 0; i < string_length; i++)
        {
            if (buffer[i] != buffer[string_length - i - 1])
            {
                isPalindrome = 0;
                break;
            }
        }

        // send the result to client
        int bytes_sent = sendto(serverfd, &isPalindrome, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (bytes_sent == -1)
        {
            printf("Error sending result to client\n");
            close(serverfd);
            return 1;
        }

        // send the length of string to client
        int size_sent = sendto(serverfd, &string_length, sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (size_sent == -1)
        {
            printf("Error in sending the buffer length to client\n");
            close(serverfd);
            return 1;
        }

        // calculate the number of occurences of each vowel
        int vowels[5] = {0};
        for (int j = 0; j < string_length; j++)
        {
            switch (buffer[j])
            {
            case 'a':
                vowels[0]++;
                break;
            case 'e':
                vowels[1]++;
                break;
            case 'i':
                vowels[2]++;
                break;
            case 'o':
                vowels[3]++;
                break;
            case 'u':
                vowels[4]++;
                break;

            default:
                break;
            }
        }

        // send the number of occurences of each vowel to the client
        int send_occr_result = sendto(serverfd, vowels, 5 * sizeof(int), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (send_occr_result == -1)
        {
            printf("Error sending number of occurences of each vowel\n");
            close(serverfd);
            return 1;
        }
    }

    close(serverfd);
    return 0;
}