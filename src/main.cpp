#define WOLFSSL_USER_SETTINGS
#include <wolfssl/wolfcrypt/settings.h>

#include "include/serverlib/server.hpp"

#include <condition_variable>
#include <csignal>
#include <iostream>
#include <thread>

HTTPServer server;
HTTPSServer ssl_server;

std::condition_variable exit_signal;
std::mutex exit_mutex;

int main() {

  signal(SIGINT, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGTERM, signal_handler);

  std::cout << "Starting HTTP server..." << std::endl;
  server.startup();
  std::cout << "Server ready." << std::endl;
  std::cout << "Starting HTTPS server..." << std::endl;
  ssl_server.startup();
  std::cout << "Server ready." << std::endl;

  std::thread http_thread(&HTTPServer::receive_connection, &server);
  std::thread https_thread(&HTTPSServer::receive_connection, &ssl_server);

  std::unique_lock<std::mutex> lock(exit_mutex);
  exit_signal.wait(lock);

  return 0;
}

void signal_handler(int signum) {
  std::cout << "Signal received: " << signum << std::endl;
  std::cout << "Closing socket..." << std::endl;
  server.close_connection();
  ssl_server.close_connection();
  std::cout << "Sockets closed." << std::endl;
  exit_signal.notify_one();
  exit(signum);
}