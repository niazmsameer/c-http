#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "library.h"

#define BACKLOG 10

int setup_instance(struct c_http_instance* instance, int port) {
    instance->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    return bind(instance->server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

_Noreturn void start_server(struct c_http_instance* instance) {
    char httpHeader[1024] = "HTTP/1.1 204 No Content\r\n\n";

    printf("Starting server\n");

    instance->is_listening = listen(instance->server_socket, BACKLOG);

    if (instance->is_listening < 0) {
        printf("An error occurred while starting the server\n");
    }

    printf("Server is listening for connections\n");

    while (1) {
        instance->client_socket = accept(instance->server_socket, NULL, NULL);
        send(instance->client_socket, httpHeader, sizeof(httpHeader), 0);
        close(instance->client_socket);
    }
}
