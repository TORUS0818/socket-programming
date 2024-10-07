#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_LIMIT 5
#define MESSAGE_SIZE 1024
#define BUFFER_SIZE (MESSAGE_SIZE + 1)

int main(int argc, char* argv[]) {
    int client_sd, server_sd; 
    struct sockaddr_in client_sa, server_sa;
    unsigned short server_port;
    unsigned int client_len;
    char receive_buffer[BUFFER_SIZE];
    int receive_message_size, send_message_size;
    
    if (argc != 2) {
        fprintf(stderr, "usage: ./server <port>\n");
        exit(EXIT_FAILURE);
    }
    if ((server_port = (unsigned short) atoi(argv[1])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    // socket
    if ((server_sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed.\n");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_sa, 0, sizeof(server_sa));
    server_sa.sin_family = AF_INET;
    server_sa.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sa.sin_port = htons(server_port);

    // bind
    if (bind(server_sd, (struct sockaddr*) &server_sa, sizeof(server_sa)) < 0) {
        perror("bind() failed.\n");
        exit(EXIT_FAILURE);
    }
    // listen
    if (listen(server_sd, QUEUE_LIMIT) < 0) {
        perror("listen() failed.\n");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        // accept
        if ((client_sd = accept(server_sd, (struct sockaddr*) &client_sa, &client_len)) < 0) {
            perror("accept() failed.\n");
            exit(EXIT_FAILURE);
        }
        printf("connected from %s.\n", inet_ntoa(client_sa.sin_addr));

        while(1) {
            //recv
            if ((receive_message_size = recv(client_sd, receive_buffer, BUFFER_SIZE, 0)) < 0) {
                perror("recv() failed.\n");
                exit(EXIT_FAILURE);
            } else if(receive_message_size == 0) {
                fprintf(stderr, "connection has already closed.\n");
                break;
            }
            //send
            if ((send_message_size = send(client_sd, receive_buffer, receive_message_size, 0)) < 0) {
                perror("send() failed.\n");
                exit(EXIT_FAILURE);
            } else if(send_message_size == 0) {
                fprintf(stderr, "connection has already closed.\n");
                break;
            }
        }
        close(client_sd);
    }
    close(server_sd);

    return EXIT_SUCCESS;
}