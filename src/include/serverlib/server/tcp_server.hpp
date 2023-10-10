#ifndef WEBSERVER_TCP_SERVER_HPP
#define WEBSERVER_TCP_SERVER_HPP

#include "../server.hpp"
#include "http_handler.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace server {

	#define MAXBUFLEN 4096
	#define HTTP_PORT 80

	template<IHandler Handler>
	class TCPServer : public std::enable_shared_from_this<TCPServer<Handler>> {
	public:
		TCPServer() : _port(HTTP_PORT) {}
		TCPServer(int&& port_str) : _port(port_str) {}

		~TCPServer() { this->Stop(); }

		void Start() {
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
			hints.ai_socktype = SOCK_STREAM; // TCP
			hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

			getaddrinfo(NULL, std::to_string(_port).c_str(), &hints, &res);

			// make a socket
			sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

			if (sockfd == -1) {
				std::cout << "Error creating socket" << std::endl;
				exit(1);
			}

			// bind it to the port we passed in to getaddrinfo()
			if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
				std::cout << "Error binding socket" << std::endl;
				exit(1);
			}

			if (listen(sockfd, MAXBUFLEN) == -1) {
				std::cout << "Error listening on socket" << std::endl;
				exit(1);
			}
		};

		void Stop() {
			close(client_fd);
			close(sockfd);
		};

		void Open() {
			AcceptIncoming();
		}

	private:
		int _port;
		std::atomic<bool> _started;

		struct addrinfo hints, *res;
		struct sockaddr_storage their_addr;
		int sockfd, client_fd;

		void AcceptIncoming() {
			socklen_t addr_size = sizeof(their_addr);

			while (true) {
				std::cout << "Waiting for connection..." << std::endl;
				client_fd = accept4(sockfd, reinterpret_cast<sockaddr *>(&their_addr),
														&addr_size, SOCK_NONBLOCK);
				if (!(client_fd == -1 && (errno == EAGAIN))) {
					std::cout << "Got connection. Receiving TLS" << std::endl;
					
					char buf[MAXBUFLEN];
					ssize_t numbytes;

					do {
						numbytes = recv(client_fd, buf, MAXBUFLEN - 1, 0);
					} while (numbytes == -1 && (errno == EAGAIN));

					if (numbytes == -1) {
						fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
						return;
					}
					buf[numbytes] = '\0';

					std::string request_str{buf};
					std::cout << "Received HTTP request: " << numbytes << std::endl;
					auto response = Handler::handle(request_str);
					
					if (response.has_value()) {
						send(client_fd, response.value().c_str(), response.value().length(), 0);
					}
					
					close(client_fd);
				}
			}
		};
	};
} // namespace server

#endif // WEBSERVER_TCP_SERVER_HPP