#include "include/serverlib/server.hpp"

#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <wolfssl/ssl.h>

#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#define BACKLOG 10
#define MAXBUFLEN 4096

void HTTPServer::close_connection() {
	close(client_fd);
	close(sockfd);
}

void HTTPSServer::close_connection() {
	close(client_fd);
	close(sockfd);
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();
}

void HTTPServer::startup() { setup(); }

void HTTPSServer::startup() {
	setup();
	load_tls();
}

void Server::setup() {
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(NULL, port.c_str(), &hints, &res);

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

void HTTPSServer::load_tls() {
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
}

void HTTPServer::accept_incoming() {
	// socklen_t addr_size;

	// addr_size = sizeof(their_addr);
	// int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
}

[[noreturn]] void Server::receive_connection() {
	socklen_t addr_size = sizeof(their_addr);

	while (true) {
		std::cout << "Waiting for connection..." << std::endl;
		client_fd = accept4(sockfd, reinterpret_cast<sockaddr *>(&their_addr),
												&addr_size, SOCK_NONBLOCK);
		if (!(client_fd == -1 && (errno == EAGAIN))) {
			std::cout << "Got connection. Receiving TLS" << std::endl;
			receive();
		}
	}
}

void HTTPServer::receive() {
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
	auto result = parse_http_request(request_str);

	if (!result) {
		std::cout << "Error parsing HTTP request" << std::endl;
		return;
	}

	auto request = result.value();

	send_html(prepare_response(request));
}

void HTTPSServer::receive() {
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
	}

	buf[numbytes] = '\0';

	if (numbytes > 0) {

		std::string request_str{buf};
		std::cout << "Received HTTP request: " << numbytes << std::endl;
		auto result = parse_http_request(request_str);

		if (!result) {
			std::cout << "Error parsing HTTP request" << std::endl;
			return;
		}

		auto request = result.value();

		send_html(HTTPServer::prepare_response(request));
	}
}

constexpr bool is_html(std::string_view str) {
	// TODO: check why ends_with is not working
	if (str.length() < 5)
		return false;
	return str.substr(str.length() - 5, 5) == ".html";
}

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

constexpr std::string remove_query(std::string str) {
	size_t query_index = str.find("?");
	if (query_index != std::string::npos) {
		return str.substr(0, query_index);
	}

	return str;
}

HTTPResponse HTTPServer::prepare_response(HTTPRequest request) {
	HTTPResponse response =
			HTTPResponse(HTTPStatusCode::OK, HTTPVersion::HTTP_1_1);

	std::filesystem::path path = "../webdir" + remove_query(request.path());
	std::cout << "Requested path: " << path << std::endl;
	auto expected_file_data = file_manager.get_file(std::filesystem::path(path));

	if(!expected_file_data.has_value()) {
		std::cerr << expected_file_data.error().what() << std::endl;
		response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTPVersion::HTTP_1_1);
		return response;
	}

	auto file_data = expected_file_data.value();

	response.SetHeader("Content-Type", decode_filetype(file_data.filetype));
	if(file_data.file_encoding != FileEncoding::NONE) {
		response.SetHeader("Content-Encoding", decode_file_encoding(file_data.file_encoding));
	}
	response.SetContent(file_data.content);

	return response;
}

void HTTPServer::send_html(HTTPResponse response) {
	std::string response_str = response_to_string(response);

	send(client_fd, response_str.c_str(), response_str.length(), 0);
	close(client_fd);
}

void HTTPSServer::send_html(HTTPResponse response) {
	std::string response_str = response_to_string(response);
	int ret, err;
	do {
		ret = wolfSSL_write(ssl, response_str.c_str(),
												static_cast<int>(response_str.length()));
		err = wolfSSL_get_error(ssl, ret);
	} while (err == WOLFSSL_ERROR_WANT_WRITE);
	wolfSSL_shutdown(ssl);
	wolfSSL_free(ssl); /* Free the wolfSSL object              */
	ssl = NULL;
}