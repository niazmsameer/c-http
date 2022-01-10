#include "library.h"

int main() {
    struct c_http_instance instance;

    int instance_setup_result = setup_instance(&instance, 8080);

    if (instance_setup_result < 0) {
        return instance_setup_result;
    }

    start_server(&instance);
}