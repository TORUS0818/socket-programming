#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "common.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sd;
    int status;
    char *node_name;
    char *service_name; 
    struct addrinfo hints;
    struct addrinfo *ai0;
    struct addrinfo *ai;
    int receive_message_size;
    char receive_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];

    if (argc != 3) {
        fprintf(stderr, "usage: ./client <hostname> <port>\n");
        exit(EXIT_FAILURE);
    }
    node_name = argv[1];
    service_name = argv[2];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(node_name, service_name, &hints, &ai0);
    if (status) {
        fprintf(stderr, "%s", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    for (ai = ai0; ai; ai = ai->ai_next) {
        // socket
        sd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sd < 0) {
            continue;
        }
        // connect
        if (connect(sd, ai->ai_addr, ai->ai_addrlen)) {
            close(sd);
            sd = -1;
            continue;
        }
        break;
    }
    if (sd < 0) {
        fprintf(stderr, "cannot connect %s.\n", node_name);
        exit(EXIT_FAILURE);
    }
    printf("connect to %s.\n", node_name);
    
    while(1) {
        printf("please enter the characters: ");
        if (fgets(send_buffer, BUFFER_SIZE, stdin) == NULL) {
            if (feof(stdin)) {
                printf("end of input detected (EOF).\n");
                break;
            } else {
                perror("error reading from stdin");
                exit(EXIT_FAILURE);
            }
        }
        // send
        send_all(sd, send_buffer, strlen(send_buffer));
        // receive
        receive_message_size = receive_all(sd, receive_buffer, BUFFER_SIZE);
        if (receive_message_size == 0) {
            printf("server disconnected.\n");
            break;
        }
        receive_buffer[receive_message_size] = '\0';
        printf("received from server: %s\n", receive_buffer);
    }
    close(sd);

    return EXIT_SUCCESS;
}
