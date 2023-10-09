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

enum class ServerType {
	HTTP,
	HTTPS
};

class Server {
public:
	// virtual void startup() = 0;
	// [[noreturn]] virtual void receive_connection();
	// const std::string& get_port_str() { return port; }
	// virtual void close_connection() = 0;

	Server() = default;
	~Server() = default;


protected:
	std::string port;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
	int sockfd, client_fd;

	// const char* get_port() { return port.c_str(); }
	// void setup();
	// virtual void receive() = 0;
};

class ServerFacade {
private:	
	Server *server;
	
public:
	ServerFacade(Server *server_in) { server = server_in; }
	~ServerFacade() = default;

	// const std::string& get_port_str() { return server->get_port_str(); }
	void startup() {
		//server->startup();
	}
	std::thread receive_connection() {
		//return std::thread(&Server::receive_connection, server);
	}
	void close_connection() {
		//server->close_connection();
	}
};

class HTTPServer : public Server {
public:
	HTTPServer() { this->port = "80"; }
	HTTPServer(std::string&& port_str) { this->port = port_str; }
	HTTPServer(const std::string& port_str) { this->port = std::string(port_str); }

	// ~HTTPServer() { close_connection(); }

	// void startup() override;
	// void receive() override;
	// std::optional<HTTPResponse> process_request(std::string& request_str);
	// static HTTPResponse prepare_response(HTTPRequest request);
	// void send_html(HTTPResponse response);
	// void close_connection() override;
};

class HTTPSServer : public Server {
private:
	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;

public:
	HTTPSServer() { this->port = "443"; }
	HTTPSServer(const std::string& port_str) { this->port = std::string(port_str); }

	// ~HTTPSServer() { close_connection(); }

	// void startup() override;
	// void load_tls();
	// void receive() override;
	// std::optional<HTTPResponse> process_request(std::string& request_str);
	// void send_html(HTTPResponse response);
	// void close_connection() override;
};

constexpr bool is_extension(std::string_view str, std::string_view extension) {
	size_t len = extension.length();

	if (str.length() < len)
		return false;
	return str.substr(str.length() - len, len) == extension;
}

static std::string supported_extensions[]{".html",  ".js",  ".css", ".png",
																					".jpg",   ".svg", ".ttf", ".woff",
																					".woff2", ".ico"};

constexpr bool is_supported_extension(std::string_view str) {
	for (int i = 0; i < 9; i++) {
		if (is_extension(str, supported_extensions[i]))
			return true;
	}

	return false;
}

#endif