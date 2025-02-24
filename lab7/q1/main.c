#include <stdio.h>
#include <stdlib.h>

int main() {
    int bucket_size = 10, input_size = 4, output_size = 1;
    int time[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int num_packets = 11; 
    int bucket = 0;
    int ticks = 1;

    int i = 0;
    while (i < num_packets) {
        if (ticks < time[i]) {
            bucket = (bucket - output_size >= 0) ? bucket - output_size : 0;
            ticks++;
        } else {
            if (bucket + input_size <= bucket_size) {
                bucket += input_size;  
                printf("Packet arrived at time %d, bucket size is %d\n", time[i], bucket);
            } else {
                printf("Packet arrived at time %d, but bucket overflows. Dropping packet\n", time[i]);
            }
            i++; 
        }
    }

    return 0;
}
