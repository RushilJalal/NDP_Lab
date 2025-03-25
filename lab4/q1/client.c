#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

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
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect to server
    int connect_ret_val = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_ret_val == -1)
    {
        printf("Error connecting to server\n");
        close(sockfd);
        return 1;
    }

    // input option from user
    int option;
    printf("Choose: \n1. Registration number\n2. Name of the student\n3. Subject code\n");
    scanf("%d", &option);
    getchar(); // consume newline character left by scanf

    // send option to server
    if (send(sockfd, &option, sizeof(option), 0) == -1)
    {
        printf("error sending option to server\n");
        close(sockfd);
        return 1;
    }

    // input chosen option data from user
    switch (option)
    {
    case 1:
        // enter reg no
        long reg_no;
        printf("Enter registration number: ");
        scanf("%ld", &reg_no);

        // send the reg no to server
        if (send(sockfd, &reg_no, sizeof(reg_no), 0) == -1)
        {
            printf("error sending reg_no to server\n");
            close(sockfd);
            return 1;
        }

        // recv whether student found
        int found;
        if (recv(sockfd, &found, sizeof(found), 0) == -1)
        {
            printf("error receiving found value from server\n");
            close(sockfd);
            return 1;
        }

        if (found == 1)
        {
            char name[50];
            char res_address[50];

            // receive name
            if (recv(sockfd, name, sizeof(name), 0) == -1)
            {
                printf("error receiving name from server\n");
                close(sockfd);
                return 1;
            }

            // receive residential address
            if (recv(sockfd, res_address, sizeof(res_address), 0) == -1)
            {
                printf("error receiving residential address from server\n");
                close(sockfd);
                return 1;
            }

            printf("Student Name: %s\n", name);
            printf("Residential Address: %s\n", res_address);
        }
        else
        {
            printf("Student not found\n");
        }

        break;

    case 2:
        // accept and send name of student
        char name[50];
        printf("Enter student name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0; // remove newline character

        // send to server
        if (send(sockfd, name, sizeof(name), 0) == -1)
        {
            printf("error sending name to server\n");
            close(sockfd);
            return 1;
        }

        // recv whether student found
        int found;
        if (recv(sockfd, &found, sizeof(found), 0) == -1)
        {
            printf("error receiving found value from server\n");
            close(sockfd);
            return 1;
        }

        // if student found, receive that student's Dept., Semester, Section and Courses Registered
        if (found == 1)
        {
            char dept[50];
            char sem[50];
            char sec[50];
            char courses[50];

            // receive dept
            if (recv(sockfd, dept, sizeof(dept), 0) == -1)
            {
                printf("error receiving dept from server\n");
                close(sockfd);
                return 1;
            }

            // receive sem
            if (recv(sockfd, sem, sizeof(sem), 0) == -1)
            {
                printf("error receiving sem from server\n");
                close(sockfd);
                return 1;
            }

            // receive sec
            if (recv(sockfd, sec, sizeof(sec), 0) == -1)
            {
                printf("error receiving sec from server\n");
                close(sockfd);
                return 1;
            }

            // receive courses
            if (recv(sockfd, courses, sizeof(courses), 0) == -1)
            {
                printf("error receiving courses from server\n");
                close(sockfd);
                return 1;
            }

            printf("Dept: %s\n", dept);
            printf("Semester: %s\n", sem);
            printf("Section: %s\n", sec);
            printf("Courses Registered: %s\n", courses);
        }
        else
        {
            printf("Student not found\n");
        }
        break;

    default:
        printf("Invalid option\n");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}