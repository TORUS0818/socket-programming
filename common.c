#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

ssize_t send_all(int s, char *buf, size_t len) {
    int sent_total = 0;
    int n;

    while (sent_total < len) {
        n = send(s, buf + sent_total, len - sent_total, 0);
        if (n == -1) { 
            perror("send() failed.\n");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            printf("end of input detected (EOF).\n");
            return sent_total;
        }
        sent_total += n;
    }
    return sent_total;
}

ssize_t receive_all(int s, char *buf, size_t len) {
    int received_total = 0;
    int n;

    while (received_total < len) {
        n = recv(s, buf + received_total, len - received_total, 0);
        if (n == -1) {
            perror("recv() failed.\n");
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            printf("end of input detected (EOF).\n");
            return received_total;
        }
        received_total += n;
        if (strchr(buf, '\n') != NULL) {
            break;
        }
    }
    return received_total;
}
