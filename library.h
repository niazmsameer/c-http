#ifndef C_HTTP_LIBRARY_H
#define C_HTTP_LIBRARY_H

struct c_http_instance {
    int server_socket;
    int client_socket;
    int is_listening;
};

int setup_instance(struct c_http_instance* instance, int port);

_Noreturn void start_server(struct c_http_instance* instance);

#endif //C_HTTP_LIBRARY_H
