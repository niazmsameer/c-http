#include <stdio.h>
#include "library.h"

void request_handler(struct request *request) {
    printf("Hello, world\n");
}

int main() {
    struct c_http_instance instance;

    int instance_setup_result = setup_instance(&instance, 8080);

    if (instance_setup_result < 0) {
        return instance_setup_result;
    }

    instance.request_handler = &request_handler;

    start_server(&instance);
}