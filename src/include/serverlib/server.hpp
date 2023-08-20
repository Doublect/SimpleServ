#ifndef SERVER_HPP
#define SERVER_HPP

#include "http.hpp"
#include "parser.hpp"

#include <wolfssl/ssl.h>

void signal_handler(int signal);

class HTTPServer {
    private: 
        int client_fd;

    public:
        HTTPServer() {}
        ~HTTPServer() { close_connection(); }

        void setup();
        void setup_tls();
        void accept_incoming();
        void load_tls();
        void receive_connection();
        void receive_http(int client_fd);
        void receive_tls(int client_fd);
        HTTPResponse prepare_response(HTTPRequest request);
        void send_html(int client_fd, HTTPResponse response);
        void send_html_tls(WOLFSSL* ssl, HTTPResponse response);
        void close_connection();
};

#endif