#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

int main()
{
    // client socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation error\n");
        return 1;
    }

    // server addr
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to the server
    // type casting is required here since the struct type and paramater type for connect is different
    int connect_return_val = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_return_val == -1)
    {
        printf("Error in connecting to server!\n");
        close(sockfd);
        return 1;
    }

    printf("Connected to server!\n");

    int buffer[BUFFER_SIZE];

    int n, i;
    printf("How many numbers?\n");
    scanf("%d", &n);
    send(sockfd, &n, sizeof(int), 0); // send no. of integers

    printf("Enter %d numbers: \n", n);
    for (i = 0; i < n; i++)
    {
        scanf("%d", &buffer[i]);
    }
    send(sockfd, buffer, n * sizeof(int), 0); // send the integers

    while (1)
    {
        int choice;
        printf("Enter your choice: \n");
        printf("1. Search for a number\n");
        printf("2. Sort the numbers in ascending/descending order\n");
        printf("3. Split to odd/even\n");
        scanf("%d", &choice);
        send(sockfd, &choice, sizeof(int), 0);

        switch (choice)
        {
        case 1:
            // search for a number
            printf("Enter the number to search: \n");
            int num, found;
            scanf("%d", &num);

            // send the number to be searched
            send(sockfd, &num, sizeof(int), 0);

            // receive the found var
            recv(sockfd, &found, sizeof(int), 0);

            if (found)
            {
                printf("Number was found\n");
            }
            else
            {
                printf("Number not found\n");
            }
            break;

        case 2:
            // sort the buffer ascending/descending
            int order;
            printf("Which order to sort in?\n");
            printf("1. Ascending\n");
            printf("2. Descending\n");
            scanf("%d", &order);

            // send the order to server
            send(sockfd, &order, sizeof(int), 0);

            // receive the sorted buffer
            recv(sockfd, buffer, n * sizeof(int), 0);

            printf("Sorted array: \n");
            for (int i = 0; i < n; i++)
            {
                printf("%d  ", buffer[i]);
            }
            printf("\n");
            break;

        case 3:
            // split buffer into odd and even arrays
            // recv the odd, even buffers and odd,even counts
            int odd[BUFFER_SIZE], even[BUFFER_SIZE], odd_count, even_count;
            recv(sockfd, &even_count, sizeof(int), 0);
            recv(sockfd, &odd_count, sizeof(int), 0);
            recv(sockfd, even, even_count * sizeof(int), 0);
            recv(sockfd, odd, odd_count * sizeof(int), 0);

            printf("Odd elements: ");
            for (int i = 0; i < odd_count; i++)
            {
                printf("%d  ", odd[i]);
            }

            printf("\nEven elements: ");
            for (int i = 0; i < even_count; i++)
            {
                printf("%d  ", even[i]);
            }
            printf("\n");
            break;
        default:
            printf("Invalid choice\n");
            close(sockfd);
            break;
        }
    }
    close(sockfd);
}