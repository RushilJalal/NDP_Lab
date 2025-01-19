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
    server_addr.sin_port = htons(8081);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to server
    int connect_ret_val = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret_val == -1)
    {
        printf("Error connecting to server\n");
        close(sockfd);
        return 1;
    }

    char buffer[256];
    while (1)
    {
        printf("Enter file name: \n");
        gets(buffer);
        // fgets(buffer, 256, stdin);

        // send file name to server
        send(sockfd, buffer, sizeof(buffer), 0);

        // recv file found result
        int file_found;
        recv(sockfd, &file_found, sizeof(int), 0);

        if (file_found == 0)
        {
            printf("File not found! Make sure it exists in the directory\n");
            continue;
        }
        else
        {
            printf("File found and opened: %s\n", buffer);
            // display menu
            int option;
            printf("Choose an option:\n");
            printf("1. Search\n2. Replace\n3. Reorder\n4. Exit\n");
            scanf("%d", &option);
            getchar(); // to consume the new line character

            // send option to server
            if (send(sockfd, &option, sizeof(int), 0) == -1)
            {
                printf("error sending option to server\n");
                close(sockfd);
                return 1;
            }

            switch (option)
            {
            case 1:
                // search a string in the file
                char string[256];
                printf("Enter string to be searched: \n");
                gets(string);

                // send string to server
                if (send(sockfd, string, sizeof(buffer), 0) == -1)
                {
                    printf("Error sending string to server\n");
                    close(sockfd);
                    return 1;
                }

                // receive count from server
                int count;
                if (recv(sockfd, &count, sizeof(int), 0) == -1)
                {
                    printf("Error receiving result from server\n");
                    close(sockfd);
                    return 1;
                }

                if (count > 0)
                {
                    printf("String count: %d\n", count);
                }
                else
                {
                    printf("String not found\n");
                }
                break;

            case 2:
                // replace str1 with str2 in the file
                char str1[50], str2[50];
                printf("Enter str1: ");
                gets(str1);
                printf("\n");
                printf("Enter str2: ");
                gets(str2);

                // send str1, str2 to server
                if (send(sockfd, str1, sizeof(str1), 0) == -1 || send(sockfd, str2, sizeof(str2), 0) == -1)
                {
                    printf("Error sending str1 or str2 to server\n");
                    close(sockfd);
                    return 1;
                }

                // receive count of replacements from server
                int count;
                if (recv(sockfd, &count, sizeof(int), 0) == -1)
                {
                    printf("error receiving count of replacements from server\n");
                    close(sockfd);
                    return 1;
                }

                if (count > 0)
                {
                    printf("String replaced\n");
                }
                else
                {
                    printf("string not found\n");
                }

                break;

            case 3:
                //rearrange the text in increasing order of ascii value
                

            default:
                break;
            }
        }
    }
}