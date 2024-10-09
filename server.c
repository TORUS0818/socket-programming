#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "common.h"

#define QUEUE_LIMIT 5
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_sd;
    int server_sd;
    int status;
    char *service_name; 
    struct addrinfo hints;
    struct addrinfo *ai0;
    struct addrinfo *ai;
    struct sockaddr_storage ss;
    unsigned int ss_len;
    int send_message_size;
    int receive_message_size;
    char receive_buffer[BUFFER_SIZE];
    
    if (argc != 2) {
        fprintf(stderr, "usage: ./server <port>\n");
        exit(EXIT_FAILURE);
    }
    service_name = argv[1];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, service_name, &hints, &ai0);
    if (status) {
        fprintf(stderr, "%s", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    for (ai = ai0; ai; ai = ai->ai_next) {
        // socket
        server_sd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (server_sd < 0) {
            continue;
        }
        // bind
        if (bind(server_sd, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("bind() failed.\n");
            close(server_sd);
            continue;
        }
        //listen
        if (listen(server_sd, QUEUE_LIMIT) < 0) {
            perror("listen() failed.\n");
            close(server_sd);
            continue;
        }
        break;
    }
    if (server_sd < 0) {
        fprintf(stderr, "cannot create server socket.\n");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        // accept
        if ((client_sd = accept(server_sd, (struct sockaddr*) &ss, &ss_len)) < 0) {
            perror("accept() failed.\n");
            exit(EXIT_FAILURE);
        }

        while(1) {
            //recv
            receive_message_size = receive_all(client_sd, receive_buffer, BUFFER_SIZE);
            if (receive_message_size == 0) {
                printf("connection has already closed.\n");
                break;
            }
            //send
            send_message_size = send_all(client_sd, receive_buffer, receive_message_size);
            if(send_message_size == 0) {
                printf("connection has already closed.\n");
                break;
            }
        }
        close(client_sd);
    }
    close(server_sd);
    
    return EXIT_SUCCESS;
}
