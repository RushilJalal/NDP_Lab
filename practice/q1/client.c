#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFER_SIZE 256

typedef struct book
{
    int ID;
    char title[20];
    char publisher[20];
    char genre[20];
} book;

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
    book books[2];

    while (1)
    {
        int choice;
        printf("Enter your choice: \n");
        printf("1. Insert\n");
        printf("2. Update\n");
        printf("3. Search\n");
        printf("4. Exit\n");
        scanf("%d", &choice);
        getchar();
        // send choice to server
        send(sockfd, &choice, sizeof(int), 0);

        // char *title, *publisher, *genre;

        switch (choice)
        {
        case 1:
            for (int i = 0; i < 2; i++)
            {
                printf("enter info for book %d", i + 1);
                books[i].ID = i;
                printf("Enter book title: ");
                gets(books[i].title);
                // strcpy(books[i].title, title);

                printf("Enter book genre: ");
                gets(books[i].genre);
                // strcpy(books[i].genre, genre);

                printf("Enter book publisher: ");
                gets(books[i].publisher);
                // strcpy(books[i].publisher, publisher);
            }

            // send books array to server
            send(sockfd, books, sizeof(books), 0);

            // display book info
            printf("Displaying book details: \n");
            for (int i = 0; i < 2; i++)
            {
                printf("Book ID: %d", books[i].ID);
                printf("Book Title: %s", books[i].title);
                printf("Genre: %s", books[i].genre);
                printf("Book Publisher: %s", books[i].publisher);
            }
            break;

        case 2:
            // update the genre of the book Harry Potter from Comic to Fantasy.
            for (int i = 0; i < 2; i++)
            {
                if (strcmp(books[i].title, "Harry Potter") == 0)
                {
                    strcpy(books[i].genre, "Fantasy");
                }
            }
            // send the books array to server
            send(sockfd, books, sizeof(books), 0);
            break;

        default:
            break;
        }

        close(sockfd);
    }
}