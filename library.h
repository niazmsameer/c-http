#ifndef C_HTTP_LIBRARY_H
#define C_HTTP_LIBRARY_H

struct header {
    char *key;
    char *value;
};

struct header_list_node {
    struct header *content;
    struct header_list_node *next_header;
};

struct header_list {
    struct header_list_node *first_header;
};

struct request {
    char path[1024];
    char method[7];
    char protocol_version[10];
    struct header_list *headers;
};

struct c_http_instance {
    int server_socket;
    int client_socket;
    int is_listening;
    void (*request_handler)(struct request *request);
};

void parse_request_buffer_first_line(struct request *request, char* request_buffer);

void handle_next_request(struct c_http_instance *instance);

int setup_instance(struct c_http_instance* instance, int port);

_Noreturn void start_server(struct c_http_instance* instance);

#endif //C_HTTP_LIBRARY_H
