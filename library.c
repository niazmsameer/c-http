#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "library.h"

#define MAX_READ_LENGTH 8096
#define CONN_BACKLOG 10

// Parsing function definitions

void parse_request_buffer(struct request *request, char *request_buffer) {
    int i, offset;

    // Read method
    for (i = 0; i < strlen(request_buffer); i++) {
        if (request_buffer[i] == ' ') {
            request->method[i] = '\0';
            break;
        }
        request->method[i] = request_buffer[i];
    }

    // Read path (Index stays the same from the last for-loop run)
    i++;
    offset = i;
    for (; i < strlen(request_buffer); i++) {
        if (request_buffer[i] == ' ') {
            request->path[i - offset] = '\0';
            break;
        }
        request->path[i - offset] = request_buffer[i];
    }

    // Read protocol version
    i++;
    offset = i;
    for (; i < strlen(request_buffer); i++) {
        if (request_buffer[i] == '\r') {
            request->protocol_version[i - offset] = '\0';
            break;
        }
        request->protocol_version[i - offset] = request_buffer[i];
    }
}

// Instance function definitions

int setup_instance(struct c_http_instance *instance, int port) {
    instance->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return bind(instance->server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

void setup_request_handler(struct c_http_instance *instance, void (*handler)(struct request *request)) {
    instance->request_handler = handler;
}

_Noreturn void start_server(struct c_http_instance *instance) {
    char response_buffer[1024] = "HTTP/1.1 204 No Content\r\n\n";

    printf("Starting server\n");

    instance->is_listening = listen(instance->server_socket, CONN_BACKLOG);

    if (instance->is_listening < 0) {
        printf("An error occurred while starting the server\n");
        exit(1);
    }

    printf("Server is listening for connections\n");

    while (1) {
        struct request *request = malloc(sizeof(struct request));
        struct header_list *header_list = malloc(sizeof(struct header_list));
        request->headers = header_list;
        char request_buffer[MAX_READ_LENGTH];

        instance->client_socket = accept(instance->server_socket, NULL, NULL);
        recv(instance->client_socket, request_buffer, MAX_READ_LENGTH, 0);

        parse_request_buffer(request, request_buffer);

        instance->request_handler(request);

        send(instance->client_socket, response_buffer, sizeof(response_buffer), 0);
        close(instance->client_socket);

        free(request);
        free(header_list);
    }
}
