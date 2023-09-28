#ifndef SERVER_HPP
#define SERVER_HPP

#include "http.hpp"
#include "parser.hpp"
#include "file_manager.hpp"

#include <thread>

#include "user_settings.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

[[noreturn]] void signal_handler(int signal);

#define HTTP_PORT "80"
#define HTTPS_PORT "443"

enum class ServerType {
	HTTP,
	HTTPS
};

class Server {
public:
	virtual void startup() = 0;
	[[noreturn]] virtual void receive_connection();
	const std::string& get_port_str() { return port; }
	virtual void close_connection() = 0;
	static FileManager file_manager;

	Server() = default;
	~Server() = default;


protected:
	std::string port;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	int sockfd, client_fd;

	const char* get_port() { return port.c_str(); }
	void setup();
	virtual void receive() = 0;
};

class ServerFacade {
private:	
	Server *server;
	
	#ifdef TEST
	void process_request(std::string& request_str) {
		server->process_request(request_str);
	}
	#endif

public:
	ServerFacade(Server *server_in) { server = server_in; }
	~ServerFacade() = default;

	const std::string& get_port_str() { return server->get_port_str(); }
	void startup() {
		server->startup();
	}
	std::thread receive_connection() {
		return std::thread(&Server::receive_connection, server);
	}
	void close_connection() {
		server->close_connection();
	}
};

class HTTPServer : public Server {
public:
	HTTPServer() { this->port = HTTP_PORT; }
	HTTPServer(std::string&& port_str) { this->port = port_str; }
	HTTPServer(const std::string& port_str) { this->port = std::string(port_str); }

	~HTTPServer() { close_connection(); }

	void startup() override;
	void accept_incoming();
	void receive() override;
	std::optional<HTTPResponse>  process_request(std::string& request_str);
	static HTTPResponse prepare_response(HTTPRequest request);
	void send_html(HTTPResponse response);
	void close_connection() override;
};

class HTTPSServer : public Server {
private:
	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;

public:
	HTTPSServer() { this->port = HTTPS_PORT; }
	HTTPSServer(const std::string& port_str) { this->port = std::string(port_str); }

	~HTTPSServer() { close_connection(); }

	void startup() override;
	void load_tls();
	void receive() override;
	std::optional<HTTPResponse> process_request(std::string& request_str);
	void send_html(HTTPResponse response);
	void close_connection() override;
};

#endif