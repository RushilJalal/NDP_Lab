#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUCKET_SIZE 10
#define PACKET_SIZE 4
#define OUTGOING_RATE 1

void leaky_bucket(int* packet_times, int num_packets) {
    int bucket = 0;
    int outgoing_time = 0;
    int conforming = 0, nonconforming = 0;
    
    for (int i = 0; i < num_packets; i++) {
        int arrival_time = packet_times[i];

        
        while (outgoing_time < arrival_time) {
            if (bucket > 0) {
                bucket--;
                printf("Time %d: Drained 1 byte from bucket. Bucket size = %d bytes\n", outgoing_time, bucket);
            }
            outgoing_time++;
        }

        if (bucket + PACKET_SIZE <= BUCKET_SIZE) {
            bucket += PACKET_SIZE;
            conforming++;
            printf("Time %d: Conforming packet of size %d bytes added to bucket. Bucket size = %d bytes\n", arrival_time, PACKET_SIZE, bucket);
        } else {
            nonconforming++;
            printf("Time %d: Nonconforming packet of size %d bytes dropped (Bucket Overflow). Bucket size = %d bytes\n", arrival_time, PACKET_SIZE, bucket);
        }
    }

    printf("\nSimulation Complete!\n");
    printf("Conforming packets: %d\n", conforming);
    printf("Nonconforming packets: %d\n", nonconforming);
}

int main() {
    int packet_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int num_packets = sizeof(packet_times) / sizeof(packet_times[0]);

    leaky_bucket(packet_times, num_packets);

    return 0;
}
