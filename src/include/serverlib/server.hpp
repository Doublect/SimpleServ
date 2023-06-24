#ifndef SERVER_HPP
#define SERVER_HPP

#include "http.hpp"

class HTTPServer {
    private: 
        int client_fd;

    public:
        HTTPServer() {}
        ~HTTPServer() {}

        void setup();
        void accept_incoming();
        void receive_connection();
        void receive_http(int client_fd);
        void send_html(int client_fd, HTTPRequest request);
};

#endif