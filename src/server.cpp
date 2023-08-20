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
#include <string>
#include <string_view>

struct sockaddr_storage their_addr;
struct addrinfo hints, *res;
struct addrinfo tls_hints, *tls_res;
int sockfd, tls_sockfd;

WOLFSSL_CTX *ctx;
WOLFSSL *ssl;

#define BACKLOG 10
#define MAXBUFLEN 4096

#define HTTP_PORT "80"
#define HTTPS_PORT "443"

void signal_handler(int signum) {
  std::cout << "Signal received: " << signum << std::endl;
  std::cout << "Closing socket..." << std::endl;
  close(sockfd);
  exit(signum);
  wolfSSL_CTX_free(ctx);
  wolfSSL_Cleanup();
}

void HTTPServer::close_connection() {
  close(client_fd);
  close(sockfd);
  wolfSSL_CTX_free(ctx);
  wolfSSL_Cleanup();
}

void HTTPServer::setup() {
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  getaddrinfo(NULL, HTTP_PORT, &hints, &res);

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

void HTTPServer::setup_tls() {
  memset(&tls_hints, 0, sizeof(tls_hints));
  tls_hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
  tls_hints.ai_socktype = SOCK_STREAM; // TCP
  tls_hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  getaddrinfo(NULL, HTTPS_PORT, &tls_hints, &tls_res);

  // make a socket
  tls_sockfd =
      socket(tls_res->ai_family, tls_res->ai_socktype, tls_res->ai_protocol);

  if (tls_sockfd == -1) {
    std::cout << "Error creating socket" << std::endl;
    exit(1);
  }

  // bind it to the port we passed in to getaddrinfo()
  if (bind(tls_sockfd, tls_res->ai_addr, tls_res->ai_addrlen) == -1) {
    std::cout << "Error binding socket" << std::endl;
    exit(1);
  }

  if (listen(tls_sockfd, MAXBUFLEN) == -1) {
    std::cout << "Error listening on socket" << std::endl;
    exit(1);
  }
}

void HTTPServer::load_tls() {
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

void HTTPServer::receive_connection() {
  socklen_t addr_size = sizeof(their_addr);

  while (true) {
    std::cout << "Waiting for connection..." << std::endl;
    // client_fd = accept4(sockfd, (struct sockaddr *)&their_addr, &addr_size,
    //                     SOCK_NONBLOCK);
    // if(!(client_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
    //   std::cout << "Got connection. Receiving HTTP" << std::endl;
    //   receive_http(client_fd);
    // }

    client_fd = accept4(tls_sockfd, (struct sockaddr *)&their_addr, &addr_size,
                        SOCK_NONBLOCK);
    if (!(client_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
      std::cout << "Got connection. Receiving TLS" << std::endl;
      receive_tls(client_fd);
    }
  }
}

void HTTPServer::receive_http(int client_fd) {
  char buf[MAXBUFLEN];
  int numbytes;

  do {
    numbytes = recv(client_fd, buf, MAXBUFLEN - 1, 0);
  } while (numbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK));

  if (numbytes == -1) {
    fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
    return;
  }
  buf[numbytes] = '\0';

  //"""GET / HTTP/1.1\r\nHost: localhost:5122\r\nUser-Agent: Mozilla/5.0 (X11;
  // Linux x86_64; rv:109.0) Gecko/2\r\n\r\nHELLO WORLD"""
  std::string request_str{buf};
  std::cout << "Received HTTP request: " << numbytes << std::endl;
  auto result = parse_http_request(request_str);

  if (!result) {
    std::cout << "Error parsing HTTP request" << std::endl;
    return;
  }

  auto request = result.value();

  send_html(client_fd, prepare_response(request));
}

void HTTPServer::receive_tls(int client_fd) {
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

    send_html_tls(ssl, prepare_response(request));
  }
}

// struct {
//     int32_t gmt_unix_time;
//     opaque random_bytes[28];
// } Random;

constexpr bool is_html(std::string_view str) {
  // TODO: check why ends_with is not working
  if (str.length() < 5)
    return false;
  return str.substr(str.length() - 5, 5) == ".html";
}

constexpr bool is_js(std::string_view str) {
  // TODO: check why ends_with is not working
  if (str.length() < 3)
    return false;
  return str.substr(str.length() - 3, 3) == ".js";
}

constexpr bool is_css(std::string_view str) {
  if (str.length() < 4)
    return false;
  return str.substr(str.length() - 4, 4) == ".css";
}

constexpr bool is_png(std::string_view str) {
  if (str.length() < 4)
    return false;
  return str.substr(str.length() - 4, 4) == ".png";
}

constexpr bool is_extension(std::string_view str, std::string_view extension) {
  int len = extension.length();

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
  int query_index = str.find("?");
  if (query_index != std::string::npos) {
    return str.substr(0, query_index);
  }

  return str;
}

HTTPResponse HTTPServer::prepare_response(HTTPRequest request) {
  HTTPResponse response =
      HTTPResponse(HTTPStatusCode::OK, HTTPVersion::HTTP_1_1);

  if (is_supported_extension(remove_query(request.path()))) {
    if (is_extension(request.path(), ".html")) {
      response.SetHeader("Content-Type", "text/html");
    } else if (is_extension(request.path(), ".js")) {
      response.SetHeader("Content-Type", "text/javascript");
    } else if (is_extension(request.path(), ".css")) {
      response.SetHeader("Content-Type", "text/css");
    } else if (is_extension(request.path(), ".png")) {
      response.SetHeader("Content-Type", "image/png");
    } else if (is_extension(request.path(), ".jpg")) {
      response.SetHeader("Content-Type", "image/jpg");
    } else if (is_extension(request.path(), ".svg")) {
      response.SetHeader("Content-Type", "image/svg+xml");
    } else if (is_extension(request.path(), ".ttf")) {
      response.SetHeader("Content-Type", "font/ttf");
    } else if (is_extension(request.path(), ".woff")) {
      response.SetHeader("Content-Type", "font/woff");
    } else if (is_extension(request.path(), ".woff2")) {
      response.SetHeader("Content-Type", "font/woff2");
    } else {
      response.SetHeader("Content-Type", "text/plain");
    }

    std::string path = "../webdir" + remove_query(request.path());

    std::ifstream file(path);
    file.open(path, std::ios::in);
    if (file.is_open()) {
      std::stringstream ss;
      ss << file.rdbuf();
      response.SetContent(ss.str());
    } else {
      std::cout << "File not found: " << path << std::endl;
      response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTPVersion::HTTP_1_1);
    }
  } else {
    response = HTTPResponse(HTTPStatusCode::SERVICE_UNAVAILABLE,
                            HTTPVersion::HTTP_1_1);
  }

  return response;
}

void HTTPServer::send_html(int client_fd, HTTPResponse response) {
  std::string response_str = response_to_string(response);

  send(client_fd, response_str.c_str(), response_str.length(), 0);
  close(client_fd);
}

void HTTPServer::send_html_tls(WOLFSSL *ssl, HTTPResponse response) {
  std::string response_str = response_to_string(response);
  int ret, err;
  do {
    ret = wolfSSL_write(ssl, response_str.c_str(), response_str.length());
    err = wolfSSL_get_error(ssl, ret);
  } while (err == WOLFSSL_ERROR_WANT_WRITE);
  wolfSSL_shutdown(ssl);
  wolfSSL_free(ssl); /* Free the wolfSSL object              */
  ssl = NULL;
}