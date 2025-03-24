/* Server Code (crc_server.c) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define CRC12_POLY 0x80F
#define CRC16_POLY 0x8005
#define CRC_CCITT_POLY 0x1021

typedef struct
{
    uint16_t crc12;
    uint16_t crc16;
    uint16_t ccitt;
} CRCValues;

uint16_t calculate_crc(const char *data, int length, uint16_t poly)
{
    uint16_t crc = (poly == CRC_CCITT_POLY) ? 0xFFFF : 0x0000;

    for (int i = 0; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            continue;
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0)
        {
            close(new_socket);
            continue;
        }

        // Extract data and CRCs
        int data_length = valread - sizeof(CRCValues);
        CRCValues received_crcs;
        memcpy(&received_crcs, buffer + data_length, sizeof(CRCValues));

        // Calculate CRCs
        uint16_t calc_crc12 = calculate_crc(buffer, data_length, CRC12_POLY);
        uint16_t calc_crc16 = calculate_crc(buffer, data_length, CRC16_POLY);
        uint16_t calc_ccitt = calculate_crc(buffer, data_length, CRC_CCITT_POLY);

        // Check results
        int valid = 1;
        if (calc_crc12 != received_crcs.crc12)
        {
            printf("CRC-12 mismatch! (Received: 0x%04X, Calculated: 0x%04X)\n",
                   received_crcs.crc12, calc_crc12);
            valid = 0;
        }
        if (calc_crc16 != received_crcs.crc16)
        {
            printf("CRC-16 mismatch! (Received: 0x%04X, Calculated: 0x%04X)\n",
                   received_crcs.crc16, calc_crc16);
            valid = 0;
        }
        if (calc_ccitt != received_crcs.ccitt)
        {
            printf("CRC-CCITT mismatch! (Received: 0x%04X, Calculated: 0x%04X)\n",
                   received_crcs.ccitt, calc_ccitt);
            valid = 0;
        }

        if (valid)
        {
            printf("Data received successfully: %.*s\n", data_length, buffer);
        }
        else
        {
            printf("Data corruption detected!\n");
        }

        close(new_socket);
    }
    return 0;
}
