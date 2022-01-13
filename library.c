#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "library.h"

#define MAX_READ_LENGTH 8096
#define CONN_BACKLOG 10

void parse_request_buffer_first_line(struct request *request, char *request_buffer) {
    // read_state can be 0, 1, 2 depending on which part of the request's first line is being read
    int read_state, write_offset;
    char *current_ptr;

    read_state = 0; write_offset = 0;
    current_ptr = request->method;
    for (int i = 0; i < strlen(request_buffer); i++) {
        if (request_buffer[i] == ' ') {
            current_ptr[i - write_offset] = '\0';
            write_offset = i + 1;
            switch (read_state) {
                case 0:
                    current_ptr = request->path;
                    read_state = 1;
                    break;
                case 1:
                    current_ptr = request->protocol_version;
                    read_state = 2;
                    break;
                default:
                    return;
            }
            continue;
        }
        current_ptr[i - write_offset] = request_buffer[i];
    }
}

int setup_instance(struct c_http_instance *instance, int port) {
    instance->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return bind(instance->server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

void handle_next_request(struct c_http_instance *instance) {
    char response_buffer[1024] = "HTTP/1.1 204 No Content\r\n\n";

    // We will define the request object in stack memory
    // so that it will be cleared when the method for handling
    // the request is complete.
    struct request request;
    struct header_list header_list;
    request.headers = &header_list;

    char request_buffer[MAX_READ_LENGTH];

    instance->client_socket = accept(instance->server_socket, NULL, NULL);
    recv(instance->client_socket, request_buffer, MAX_READ_LENGTH, 0);

    parse_request_buffer_first_line(&request, request_buffer);

    instance->request_handler(&request);

    send(instance->client_socket, response_buffer, sizeof(response_buffer), 0);
    close(instance->client_socket);
}

_Noreturn void start_server(struct c_http_instance *instance) {
    printf("Starting server\n");

    instance->is_listening = listen(instance->server_socket, CONN_BACKLOG);

    if (instance->is_listening < 0) {
        printf("An error occurred while starting the server\n");
        exit(1);
    }

    printf("Server is listening for connections\n");

    while (1) {
        handle_next_request(instance);
    }
}
