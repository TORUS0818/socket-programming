#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MESSAGE_SIZE 1024
#define BUFFER_SIZE (MESSAGE_SIZE + 1)

int main(int argc, char* argv[]) {
    int sd;
    struct sockaddr_in server_sa;
    unsigned short server_port;
    char receive_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];

    if (argc != 3) {
        fprintf(stderr, "usage: ./client <hostname> <port>\n");
        exit(EXIT_FAILURE);
    }
    if (inet_aton(argv[1], &server_sa.sin_addr) == 0) {
        fprintf(stderr, "invalid IP address.\n");
        exit(EXIT_FAILURE);
    }
    if ((server_port = (unsigned short) atoi(argv[2])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_sa, 0, sizeof(server_sa));
    server_sa.sin_family = AF_INET;
    server_sa.sin_port = htons(server_port);

    // socket
    if ((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed.\n");
        exit(EXIT_FAILURE);
    }
    // connect
    if (connect(sd, (struct sockaddr*) &server_sa, sizeof(server_sa))) {
        perror("connect() failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("connect to %s.\n", inet_ntoa(server_sa.sin_addr));
    
    while(1) {
        printf("please enter the characters: ");
        if (fgets(send_buffer, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "invalid input.\n");
            exit(EXIT_FAILURE);
        }
        // send
        if (send(sd, send_buffer, strlen(send_buffer), 0) <= 0) {
            perror("send() failed.\n");
            exit(EXIT_FAILURE);
        }

        int received_byte = 0;
        int current_byte = 0;
        while(current_byte < BUFFER_SIZE) {
            // recv
            received_byte = recv(sd, &receive_buffer[current_byte], 1, 0);
            if (received_byte < 0) {
                perror("recv() failed.\n");
                exit(EXIT_FAILURE);
            } else if (received_byte == 0) {
                perror("ERR_EMPTY_RESPONSE");
                exit(EXIT_FAILURE);
            }

            if (receive_buffer[current_byte] == '\n') {
                receive_buffer[current_byte] = '\0';
                if (strcmp(receive_buffer, "quit") == 0) {
                    close(sd);
                    return EXIT_SUCCESS;
                }
                break;
            }
            current_byte += received_byte;
        }
        printf("received from server: %s\n", receive_buffer);
    }
    return EXIT_SUCCESS;
}