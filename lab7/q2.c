#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REPLENISH_RATE 10
#define BUCKET_SIZE 50
#define PACKET_SIZE 15
#define SEND_INTERVAL 0.5

void token_bucket_simulation_1() {
    int tokens = BUCKET_SIZE;
    double time = 0.0;
    int packets_sent = 0;

    while (time <= 1.5) {
        tokens += REPLENISH_RATE * 0.5;
        if (tokens > BUCKET_SIZE) {
            tokens = BUCKET_SIZE;
        }

        printf("Time %.1f sec: Tokens in bucket = %d KB\n", time, tokens);

        if ((time > 0) && (time - (packets_sent * SEND_INTERVAL) >= SEND_INTERVAL)) {
            if (tokens >= PACKET_SIZE) {
                tokens -= PACKET_SIZE;
                printf("Packet of 15 KB sent at time %.1f sec. Tokens left: %d KB\n", time, tokens);
            } else {
                printf("Packet of 15 KB queued or dropped at time %.1f sec. Tokens left: %d KB\n", time, tokens);
            }
            packets_sent++;
        }

        time += 0.5;
        usleep(500000);
    }
    printf("%d tokens left after 1.5 seconds\n", tokens);
}

void token_bucket_simulation_2() {
    int tokens = BUCKET_SIZE;
    double time = 0.0;
    int packets_sent = 0;

    while (1) {
        tokens += REPLENISH_RATE * 0.5;
        if (tokens > BUCKET_SIZE) {
            tokens = BUCKET_SIZE;
        }

        printf("Time %.1f sec: Tokens in bucket = %d KB\n", time, tokens);

        if ((time > 0) && (time - (packets_sent * SEND_INTERVAL) >= SEND_INTERVAL)) {
            if (tokens >= PACKET_SIZE) {
                tokens -= PACKET_SIZE;
                printf("Packet of 15 KB sent at time %.1f sec. Tokens left: %d KB\n", time, tokens);
            } else {
                printf("Packet of 15 KB queued or dropped at time %.1f sec. Tokens left: %d KB\n", time, tokens);
                break;
            }
            packets_sent++;
        }

        time += 0.5;
        usleep(500000);
    }

    printf("Time till packets are queued or dropped, %.lf\n", time);
}

int main() {
    token_bucket_simulation_1();

    token_bucket_simulation_2();


    // Maximum possible burst size with max rate R = 20 KBps (rate limit) is 50 KB.
    // This can be inferred from the bucket size (as the token bucket limits the burst size).
    printf("\nMaximum possible burst size with maximum rate R = 20 KBps is 50 KB.\n");

    return 0;
}
