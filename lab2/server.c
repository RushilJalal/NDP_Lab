#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int file_server(int clientfd)
{
    char buffer[256];

    while (1)
    {
        // recv filename from client
        int recv_value = recv(clientfd, buffer, sizeof(buffer), 0);

        printf("File name received from client: %s\n", buffer);

        // create file pointer
        FILE *fp = fopen(buffer, "r+");
        int file_found = 1;

        if (!fp)
        {
            file_found = 0;
        }

        // send result back to client
        send(clientfd, &file_found, sizeof(int), 0);

        if (file_found == 0)
            continue;

        else
        {
            // receive option
            int option;
            if (recv(clientfd, &option, sizeof(int), 0) == -1)
            {
                printf("Error receiving option from client\n");
                close(clientfd);
                return 1;
            }

            printf("option received: %d\n", option);

            switch (option)
            {
            case 1:
                // recv string from client
                char string[256];
                if (recv(clientfd, string, sizeof(string), 0) == -1)
                {
                    printf("Error receiving string from client\n");
                    close(clientfd);
                    return 1;
                }

                // search for string in file line by line
                char line[1024];
                int count = 0;
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    char *pos = line; // points to beginning of line
                    while ((pos = strstr(pos, string)) != NULL)
                    {
                        count++;
                        pos += strlen(string);
                    }
                }
                fclose(fp);

                // send string count to the client
                if (send(clientfd, &count, sizeof(int), 0) == -1)
                {
                    printf("Error sending string count result\n");
                    close(clientfd);
                    return 1;
                }

                printf("Sent result to client\n");
                break;

            case 2:
            //replace str1 with str2
            

            default:
                break;
            }
        }
    }
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
    server_addr.sin_port = htons(8081);
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

    while (1)
        file_server(clientfd);

    return 0;
}