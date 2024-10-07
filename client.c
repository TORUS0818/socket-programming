#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define MESSAGE_SIZE 1024
#define BUFFER_SIZE (MESSAGE_SIZE + 1)

int main(int argc, char* argv[]) {
    int sd;
    int g;
    char* node_name;
    char* service_name; 
    struct addrinfo hints;
    struct addrinfo* ai0;
    struct addrinfo* ai;
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

    g = getaddrinfo(node_name, service_name, &hints, &ai0);
    if (g) {
        fprintf(stderr, "%s", gai_strerror(g));
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
