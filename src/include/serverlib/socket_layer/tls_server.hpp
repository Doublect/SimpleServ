#ifndef WEBSERVER_TLS_SERVER_HPP
#define WEBSERVER_TLS_SERVER_HPP

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
#include <wolfssl/ssl.h>

#define MAXBUFLEN 4096
#define HTTP_PORT 80

template<IHandler Handler>
class TLSServer : public std::enable_shared_from_this<TLSServer<Handler>> {
public:
	TLSServer() : _port(HTTP_PORT) {}
	TLSServer(int&& port_str) : _port(port_str) {}

	~TLSServer() { this->Stop(); }

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
	};

	void Stop() {
		close(client_fd);
		close(sockfd);
		wolfSSL_CTX_free(ctx);
		wolfSSL_Cleanup();
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

	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;

	void AcceptIncoming() {
		socklen_t addr_size = sizeof(their_addr);

		while (true) {
			std::cout << "Waiting for connection..." << std::endl;
			client_fd = accept4(sockfd, reinterpret_cast<sockaddr *>(&their_addr),
													&addr_size, SOCK_NONBLOCK);
			if (!(client_fd == -1 && (errno == EAGAIN))) {
				std::cout << "Got connection. Receiving TLS" << std::endl;
			
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

				if (numbytes > 0) {
					std::string request_str{buf};
					auto response = Handler::handle(request_str);
				
					if (response.has_value()) {
						int err;
						do {
							ret = wolfSSL_write(ssl, response.value().c_str(),
																	static_cast<int>(response.value().length()));
							err = wolfSSL_get_error(ssl, ret);
						} while (err == WOLFSSL_ERROR_WANT_WRITE);
						wolfSSL_shutdown(ssl);
						wolfSSL_free(ssl); /* Free the wolfSSL object              */
						ssl = NULL;
					}
				}
			}
		}
	};
};

#endif // WEBSERVER_TLS_SERVER_HPP