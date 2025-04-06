#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef struct course
{
    char subject_name[20];
    int marks;
} course;

typedef struct student
{
    long reg_no;
    char name[50];
    char sub_code[20];
    char res_address[50];
    char dept[20];
    int sem;
    char section;
    course subjects[10];
} student;

int main()
{
    student students[2] = {
        {
            12345678,                                        // reg_no
            "John Doe",                                      // name
            "CS101",                                         // sub_code
            "123 Main St, Springfield",                      // res_address
            "Computer Science",                              // dept
            5,                                               // sem
            'A',                                             // section
            {{"OS", 85}, {"DS", 90}, {"CN", 88}, {"SE", 92}} // subjects
        },

        {
            98765432,                                              // reg_no
            "Jane Smith",                                          // name
            "MTH202",                                              // sub_code
            "456 Oak Rd, Rivertown",                               // res_address
            "Mathematics",                                         // dept
            3,                                                     // sem
            'B',                                                   // section
            {{"Calculus", 95}, {"LA", 89}, {"PT", 91}, {"AI", 88}} // subjects
        }};

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation error\n");
        return 1;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
    {
        printf("error setting socket options\n");
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

    // recv option from client
    int option;
    if (recv(clientfd, &option, sizeof(option), 0) == -1)
    {
        printf("error receiving option from client\n");
        close(sockfd);
        close(clientfd);
        return 1;
    }

    switch (option)
    {
    case 1:
        // recv reg_no
        long reg_no;
        if (recv(clientfd, &reg_no, sizeof(reg_no), 0) == -1)
        {
            printf("error receiving reg_no from client\n");
            close(sockfd);
            close(clientfd);
            return 1;
        }

        // create child process
        pid_t pid = fork();

        if (pid == 0)
        {
            // child process
            // check if reg_no exists in any struct and display name and address

            int found = 0;
            int i;
            for (i = 0; i < 2; i++)
            {
                if (students[i].reg_no == reg_no)
                {
                    found = 1;
                    break;
                }
            }

            // send found to client
            if (send(clientfd, &found, sizeof(found), 0) == -1)
            {
                printf("error sending found result to client\n");
                close(sockfd);
                close(clientfd);
                return 1;
            }

            // if student found, send name, address
            if (found == 1)
            {
                if (send(clientfd, students[i].name, sizeof(students[i].name), 0) == -1)
                {
                    printf("error sending name to client\n");
                    close(sockfd);
                    close(clientfd);
                    return 1;
                }

                if (send(clientfd, students[i].res_address, sizeof(students[i].res_address), 0) == -1)
                {
                    printf("error sending residential address to client\n");
                    close(sockfd);
                    close(clientfd);
                    return 1;
                }
            }
        }
        break;

    case 2:
        // recv name of student
        char name[50];
        if (recv(clientfd, name, sizeof(name), 0) == -1)
        {
            printf("error receiving name from client\n");
            close(sockfd);
            close(clientfd);
            return 1;
        }

        // handle the received name by sending back that student's details
        int found = 0;
        int i;
        for (i = 0; i < 2; i++)
        {
            if (strcmp(students[i].name, name) == 0)
            {
                found = 1;
                break;
            }
        }

        // send found to client
        if (send(clientfd, &found, sizeof(found), 0) == -1)
        {
            printf("error sending found result to client\n");
            close(sockfd);
            close(clientfd);
            return 1;
        }

        // if student found, send that student's Dept., Semester, Section and Courses Registered
        if (found == 1)
        {
            if (send(clientfd, students[i].dept, sizeof(students[i].dept), 0) == -1)
            {
                printf("error sending department to client\n");
                close(sockfd);
                close(clientfd);
                return 1;
            }

            if (send(clientfd, &students[i].sem, sizeof(students[i].sem), 0) == -1)
            {
                printf("error sending semester to client\n");
                close(sockfd);
                close(clientfd);
                return 1;
            }

            if (send(clientfd, &students[i].section, sizeof(students[i].section), 0) == -1)
            {
                printf("error sending section to client\n");
                close(sockfd);
                close(clientfd);
                return 1;
            }

            // send courses registered
            for (int j = 0; j < 4; j++)
            {
                if (send(clientfd, students[i].subjects[j].subject_name, sizeof(students[i].subjects[j].subject_name), 0) == -1)
                {
                    printf("error sending subject name to client\n");
                    close(sockfd);
                    close(clientfd);
                    return 1;
                }

                if (send(clientfd, &students[i].subjects[j].marks, sizeof(students[i].subjects[j].marks), 0) == -1)
                {
                    printf("error sending marks to client\n");
                    close(sockfd);
                    close(clientfd);
                    return 1;
                }
            }
        }


    default:
        printf("Invalid option received\n");
        close(sockfd);
        close(clientfd);
        return 1;
    }

    close(sockfd);
    return 0;
}