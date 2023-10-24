#include <serverlib/server/tcp_server.hpp>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <wolfssl/ssl.h>

#include <charconv>


void TCPServer::Start() {
	auto self(shared_from_this());

	auto start_handler = [this, self]() {
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
	}

	_strand.post()
}

