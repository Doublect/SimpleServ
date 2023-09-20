#define WOLFSSL_USER_SETTINGS
#include <wolfssl/wolfcrypt/settings.h>

#include "include/serverlib/server_manager.hpp"
#include "include/serverlib/content_coding.hpp"
#include "include/serverlib/file_manager.hpp"

#include <condition_variable>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

std::condition_variable exit_signal;
std::mutex exit_mutex;

using namespace std::literals::string_literals;
ServerManager server_manager;

int main() {
	Server::file_manager.construct_file_descriptors("../webdir");
	Server::file_manager.generate_encoded_files();

	server_manager = ServerManager(
			std::vector{ServerConfig{"HTTP"s, ServerType::HTTP, "80"s},
			ServerConfig{"HTTPS"s, ServerType::HTTPS, "443"s}});

	signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);
	// std::cout << "Starting HTTP server..." << std::endl;
	// server.startup();
	// std::cout << "Server ready." << std::endl;
	// std::cout << "Starting HTTPS server..." << std::endl;
	// ssl_server.startup();
	// std::cout << "Server ready." << std::endl;

	// std::thread http_thread(&Server::receive_connection, &server);
	// std::thread https_thread(&Server::receive_connection, &ssl_server);

	std::unique_lock<std::mutex> lock(exit_mutex);
	exit_signal.wait(lock);

	return 0;
}

[[noreturn]] void signal_handler(int signum) {
	server_manager.~ServerManager();
	exit_signal.notify_one();
	exit(signum);
};
