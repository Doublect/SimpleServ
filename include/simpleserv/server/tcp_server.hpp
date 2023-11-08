#ifndef WEBSERVER_TCP_SERVER_HPP
#define WEBSERVER_TCP_SERVER_HPP

#include <simpleserv/http/http_handler.hpp>
#include <simpleserv/request_processing/connection_handler.hpp>
#include <simpleserv/request_processing/tcp_connection.hpp>
#include <simpleserv/request_processing/threadpool.hpp>
#include <simpleserv/server/constants.hpp>
#include <simpleserv/server/server.hpp>
#include <simpleserv/utility/logger.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace server {
	template<IHandler Handler>
	class TCPServer : public Server, public std::enable_shared_from_this<TCPServer<Handler>> {
	public:
		TCPServer() : Server(HTTP_PORT) {}
		explicit TCPServer(uint32_t port) : Server(port) {}

		TCPServer(const TCPServer&) = delete;
		TCPServer& operator=(const TCPServer&) = delete;
		TCPServer(TCPServer&&) = delete;
		TCPServer& operator=(TCPServer&&) = delete;

		~TCPServer() override { this->Stop(); }

		void Start() override {
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
			hints.ai_socktype = SOCK_STREAM; // TCP
			hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

			const int gai = getaddrinfo(nullptr, std::to_string(_port).c_str(), &hints, &res);
			if (gai != 0) {
					utility::Logger::fatal(std::format("getaddrinfo: {}", gai_strerror(gai)));
					quick_exit(EXIT_FAILURE);
			}

			// make a socket
			sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			fcntl(sockfd, F_SETFL, O_NONBLOCK);

			if (sockfd == -1) {
				utility::Logger::fatal("Error creating socket");
				quick_exit(1);
			}

			// bind it to the port we passed in to getaddrinfo()
			if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
				utility::Logger::fatal("Error binding socket");
				quick_exit(1);
			}

			if (listen(sockfd, MAXBUFLEN) == -1) {
				utility::Logger::fatal("Error listening on socket");
				quick_exit(1);
			}

			_threadpool.Start();
		};

		void Stop() override {
			close(client_fd);
			close(sockfd);
		};

		void Open(std::stop_token stoken) override {
			AcceptIncoming(stoken);
		}

	private:
		request_processing::Threadpool<4, request_processing::HTTPConnectionObject, request_processing::ThreadpoolConnectionHandler<http::HTTPHandler, request_processing::HTTPConnectionObject, request_processing::TCPSender, request_processing::TCPCloser>> _threadpool;
		
		std::atomic<bool> _started = false;

		struct addrinfo hints{}, *res{};
		struct sockaddr_storage their_addr{};
		int sockfd = -1, client_fd = -1;


		void AcceptIncoming(const std::stop_token &stoken) {
			sockaddr client_addr{};
			socklen_t addr_size = sizeof(their_addr);

			std::cout << "Waiting for connection...\n";
			while (true) {
				if(stoken.stop_requested()) {
					return;
				}
				client_fd = accept4(sockfd, &client_addr,
														&addr_size, SOCK_NONBLOCK);
				if (client_fd != -1 && errno != EAGAIN) {
					std::cout << "Got connection. Receiving TCP\n";
					
					std::array<char, MAXBUFLEN> buf{};
					ssize_t numbytes = recv(client_fd, buf.data(), MAXBUFLEN - 1, 0);
	
					while (numbytes == -1 && (errno == EAGAIN)) {
						numbytes = recv(client_fd, buf.data(), MAXBUFLEN - 1, 0);
					}


					if (numbytes == -1) {
						utility::Logger::error(std::format("recv: {} ({})\n", strerror(errno), errno));
						return;
					}
					buf.at(static_cast<size_t>(numbytes)) = '\0';

					const std::string request_str{buf.data()};
					
					_threadpool.add_task(request_processing::HTTPConnectionObject{.client_fd = client_fd, .request = request_str});
				}
			}
		};
	};

} // namespace server

#endif // WEBSERVER_TCP_SERVER_HPP