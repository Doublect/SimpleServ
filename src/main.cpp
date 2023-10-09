#include "include/serverlib/socket_layer/http_handler.hpp"
#include "include/serverlib/socket_layer/tcp_server.hpp"
#include "include/serverlib/socket_layer/tls_server.hpp"
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
	file_manager.construct_file_descriptors("../webdir");
	file_manager.generate_encoded_files();

	// server_manager = ServerManager(
	// 		std::vector{ServerConfig{"HTTP"s, ServerType::HTTP, "80"s},
	// 		ServerConfig{"HTTPS"s, ServerType::HTTPS, "443"s}});
	std::cout << "Starting HTTP server..." << std::endl;
	TCPServer<HTTPHandler> server{8080};
	TLSServer<HTTPHandler> server_tls{443};

	//server.Start();
	server_tls.Start();

	signal(SIGINT, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGTERM, signal_handler);

	std::unique_lock<std::mutex> lock(exit_mutex);

	//server.Open();
	server_tls.Open();
	exit_signal.wait(lock);

	return 0;
}

[[noreturn]] void signal_handler(int signum) {
	server_manager.~ServerManager();
	exit_signal.notify_one();
	exit(signum);
};
