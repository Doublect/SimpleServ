#ifndef WEBSERVER_TLS_SERVER_HPP
#define WEBSERVER_TLS_SERVER_HPP

#include <simpleserv/http/http_handler.hpp>
#include <simpleserv/request_processing/connection_handler.hpp>
#include <simpleserv/request_processing/threadpool.hpp>
#include <simpleserv/request_processing/tls_connection.hpp>
#include <simpleserv/server/server.hpp>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <wolfssl/ssl.h>

#define MAXBUFLEN 4096
#define HTTPS_PORT 443

namespace server {

	template<IHandler Handler>
	class TLSServer : public Server, public std::enable_shared_from_this<TLSServer<Handler>> {
	public:
		TLSServer() {
			_port = HTTPS_PORT;
		}
		TLSServer(int&& port_str) {
			_port = port_str;
		}

		~TLSServer() { this->Stop(); }

		void Start() override {
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
			hints.ai_socktype = SOCK_STREAM; // TCP
			hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

			getaddrinfo(NULL, std::to_string(_port).c_str(), &hints, &res);

			// make a socket
			sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			fcntl(sockfd, F_SETFL, O_NONBLOCK);

			if (sockfd == -1) {
				std::cout << "Error creating socket\n";
				exit(1);
			}

			// bind it to the port we passed in to getaddrinfo()
			if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
				std::cout << "Error binding socket\n";
				exit(1);
			}

			if (listen(sockfd, MAXBUFLEN) == -1) {
				std::cout << "Error listening on socket\n";
				exit(1);
			}

			wolfSSL_Init();

			if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
				fprintf(stderr, "wolfSSL_CTX_new error.\n");
				exit(EXIT_FAILURE);
			}

			// /* Load CA certificates into CYASSL_CTX */
			// if (wolfSSL_CTX_load_verify_locations(ctx, "../certs/ca.crt", 0) !=
			//     SSL_SUCCESS) {
			//   fprintf(stderr, "Error loading ../certs/ca.crt, "
			//                   "please check the file.\n");
			//   exit(EXIT_FAILURE);
			// }

			/* Load server certificates into WOLFSSL_CTX */
			if (wolfSSL_CTX_use_certificate_file(ctx, "../certs/linor_com.crt",
																					SSL_FILETYPE_PEM) != SSL_SUCCESS) {
				fprintf(stderr, "Error loading ../certs/linor_com.crt, please\
						check the file.\n");
				exit(EXIT_FAILURE);
			}

			/* Load keys */
			if (wolfSSL_CTX_use_PrivateKey_file(ctx, "../certs/server.key",
																					SSL_FILETYPE_PEM) != SSL_SUCCESS) {
				fprintf(stderr, "Error loading ../certs/server.key, please check\
						the file.\n");
				exit(EXIT_FAILURE);
			}

			_threadpool.Start();
		};

		void Stop() override {
			close(client_fd);
			close(sockfd);
			wolfSSL_CTX_free(ctx);
			wolfSSL_Cleanup();
		};

		void Open(std::stop_token stoken) override {
			AcceptIncoming(stoken);
		}

	private:
		std::atomic<bool> _started = false;

		struct addrinfo hints, *res;
		struct sockaddr_storage their_addr;
		int sockfd, client_fd;

		WOLFSSL_CTX* ctx = nullptr;
		WOLFSSL* ssl = nullptr;

		request_processing::Threadpool<4, request_processing::HTTPSConnectionObject, request_processing::ThreadpoolConnectionHandler<http::HTTPHandler, request_processing::HTTPSConnectionObject, request_processing::TLSSender, request_processing::TLSCloser>> _threadpool;

		void AcceptIncoming(std::stop_token stoken) {
			socklen_t addr_size = sizeof(their_addr);

			std::cout << "Waiting for connection...\n";
			while (true) {
				if(stoken.stop_requested()) {
					return;
				}
				client_fd = accept4(sockfd, reinterpret_cast<sockaddr *>(&their_addr),
														&addr_size, SOCK_NONBLOCK);
				if (!(client_fd == -1 && (errno == EAGAIN))) {
					std::cout << "Got connection. Receiving TLS\n";
				
					char buf[MAXBUFLEN];
					int numbytes;
					int ret;

					if ((ssl = wolfSSL_new(ctx)) == NULL) {
						fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
						ret = -1;
					}

					wolfSSL_set_fd(ssl, client_fd);

					ret = wolfSSL_accept(ssl);

					int err;
					do {
						numbytes = wolfSSL_read(ssl, buf, (sizeof(buf) - 1));
						err = wolfSSL_get_error(ssl, ret);
					} while (err == WOLFSSL_ERROR_WANT_READ);

					if (numbytes < 0) {
						(wolfSSL_ERR_print_errors_fp(stdout, wolfSSL_get_error(ssl, numbytes)));
						return;
					}

					buf[numbytes] = '\0';

					std::string request_str{buf};

					_threadpool.add_task(request_processing::HTTPSConnectionObject{.client_fd = client_fd, .ssl = ssl, .request = request_str});
				}
			}
		};
	};

} // namespace server

#endif // WEBSERVER_TLS_SERVER_HPP