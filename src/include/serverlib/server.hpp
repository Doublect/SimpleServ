#ifndef SERVER_HPP
#define SERVER_HPP

#include "http.hpp"
#include "parser.hpp"

#include <wolfssl/ssl.h>

[[noreturn]] void signal_handler(int signal);

#define HTTP_PORT "80"
#define HTTPS_PORT "443"

class Server {
	public:
	[[noreturn]] void receive_connection();

protected:
	std::string port;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	int sockfd, client_fd;

	const char* get_port() { return port.c_str(); }
	void setup();
	virtual void receive() = 0;
};

class HTTPServer : public Server {
public:
	HTTPServer() { this->port = HTTP_PORT; }
	~HTTPServer() { close_connection(); }

	void startup();
	void accept_incoming();
	void receive() override;
	static HTTPResponse prepare_response(HTTPRequest request);
	void send_html(HTTPResponse response);
	void close_connection();
};

class HTTPSServer : public Server {
private:
	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;

public:
	HTTPSServer() { this->port = HTTPS_PORT; }
	~HTTPSServer() { close_connection(); }

	void startup();
	void load_tls();
	[[noreturn]] void receive_connection();
	void receive() override;
	void send_html(HTTPResponse response);
	void close_connection();
};

#endif