#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SIZE 1024
#define BACKLOG 10

_Noreturn void start_server() {
    char httpHeader[8000] = "HTTP/1.1 204 No Content\r\n\n";

    printf("Starting server\n");

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    int is_listening = listen(server_socket, BACKLOG);

    if (is_listening < 0) {
        printf("An error occurred while starting the server\n");
    }

    int client_socket;

    printf("Server is accepting connections\n");

    while (1) {
        client_socket = accept(server_socket, NULL, NULL);
        send(client_socket, httpHeader, sizeof(httpHeader), 0);
        close(client_socket);
    }
}
