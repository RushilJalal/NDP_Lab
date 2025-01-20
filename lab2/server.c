#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int file_server(int clientfd)
{
    char filename[256];

    while (1)
    {
        // recv filename from client
        int recv_value = recv(clientfd, filename, sizeof(filename), 0);

        printf("File name received from client: %s\n", filename);

        // create file pointer
        FILE *fp = fopen(filename, "r+");
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
                // replace str1 with str2
                // recv str1 and str2
                char str1[50], str2[50];
                if (recv(clientfd, str1, sizeof(str1), 0) == -1 || recv(clientfd, str2, sizeof(str2), 0) == -1)
                {
                    printf("Error receiving str1, str2\n");
                    close(clientfd);
                    return 1;
                }

                // create a temporary file to store modified content
                FILE *temp = fopen("temp.txt", "w");
                if (temp == NULL)
                {
                    printf("Error creating temporary file\n");
                    fclose(fp);
                    close(clientfd);
                    return 1;
                }

                // search for str1 in file line by line and replace with str2
                count = 0;
                memset(line, 0, sizeof(line));

                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    char *pos = line; // points to beginning of line
                    while ((pos = strstr(pos, str1)) != NULL)
                    {
                        count++;
                        char buffer[1024];
                        strncpy(buffer, line, pos - line);
                        buffer[pos - line] = '\0';
                        strcat(buffer, str2);
                        strcat(buffer, pos + strlen(str1));
                        strcpy(line, buffer);
                        pos += strlen(str2);
                    }
                    fputs(line, temp);
                }

                // send the count back to client
                if (send(clientfd, &count, sizeof(int), 0) == -1)
                {
                    printf("error sending count of replacements to client\n");
                    close(clientfd);
                    return 1;
                }

                fclose(fp);
                fclose(temp);

                remove(filename);
                rename("temp.txt", filename);
                break;

            case 3:
                // rearrange the entire text in the file in increasing order of their ASCII value
                fseek(fp, 0, SEEK_END);
                long file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                char *file_content = malloc(file_size + 1);
                if (file_content == NULL)
                {
                    printf("Memory allocation error\n");
                    fclose(fp);
                    close(clientfd);
                    return 1;
                }

                fread(file_content, 1, file_size, fp);
                file_content[file_size] = '\0';

                // sort the file content
                qsort(file_content, file_size, sizeof(char), (int (*)(const void *, const void *))strcmp);

                // write the sorted content back to the file
                freopen(filename, "w", fp);
                fputs(file_content, fp);

                free(file_content);
                fclose(fp);
                break;

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