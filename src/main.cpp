#include "include/serverlib/server.hpp"

#include <iostream>

int main() {
    HTTPServer server;

    std::cout << "Starting server..." << std::endl;
    server.setup();
    std::cout << "Server started." << std::endl;
    server.accept_incoming();
    std::cout << "Connection accepting setup." << std::endl;
    server.receive_connection();

    return 0;
}