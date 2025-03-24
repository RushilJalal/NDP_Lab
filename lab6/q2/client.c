/* Client Code (crc_client.c) */
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

int main(int argc, char const *argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if (argc != 2)
    {
        printf("Usage: %s <message>\n", argv[0]);
        return -1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Prepare message with all three CRCs
    int data_length = strlen(argv[1]);
    strncpy(buffer, argv[1], BUFFER_SIZE - sizeof(CRCValues));

    CRCValues crcs;
    crcs.crc12 = calculate_crc(buffer, data_length, CRC12_POLY);
    crcs.crc16 = calculate_crc(buffer, data_length, CRC16_POLY);
    crcs.ccitt = calculate_crc(buffer, data_length, CRC_CCITT_POLY);

    memcpy(buffer + data_length, &crcs, sizeof(CRCValues));

    send(sock, buffer, data_length + sizeof(CRCValues), 0);
    printf("Message sent with CRCs:\n");
    printf("CRC-12: 0x%04X\n", crcs.crc12);
    printf("CRC-16: 0x%04X\n", crcs.crc16);
    printf("CRC-CCITT: 0x%04X\n", crcs.ccitt);

    close(sock);
    return 0;
}
