#ifndef SERVER_HPP
#define SERVER_HPP

#include "http.hpp"
#include "parser.hpp"

#include <wolfssl/ssl.h>

void signal_handler(int signal);

#define HTTP_PORT "80"
#define HTTPS_PORT "443"

class Server {
    protected:
        std::string port;
        struct addrinfo hints, *res;
        struct sockaddr_storage their_addr;
        int sockfd;

        char *get_port() { return (char *)port.c_str(); };
        void setup(const char *port);
};

class HTTPServer : public Server {
    private: 
        int client_fd;

    public:
        HTTPServer() {
            this->port = HTTP_PORT;
        }
        ~HTTPServer() { close_connection(); }

        void startup();
        void accept_incoming();
        void receive_connection();
        void receive(int client_fd);
        static HTTPResponse prepare_response(HTTPRequest request);
        void send_html(int client_fd, HTTPResponse response);
        void close_connection();
};

class HTTPSServer : public Server {
    private: 
        int client_fd;
        WOLFSSL_CTX *ctx;
        WOLFSSL *ssl;
    
    public:
        HTTPSServer() {
            this->port = HTTPS_PORT;
        }
        ~HTTPSServer() { close_connection(); }

        void startup();
        void load_tls();
        void receive_connection();
        void receive(int client_fd);
        void send_html(WOLFSSL* ssl, HTTPResponse response);
        void close_connection();
};

#endif