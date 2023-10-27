#include <simpleserv/http/http_handler.hpp>
#include <simpleserv/server/server.hpp>
#include <simpleserv/server/tcp_server.hpp>
#include <simpleserv/server/tls_server.hpp>
#include <simpleserv/server_manager.hpp>
#include <simpleserv/file_manager.hpp>

#include <condition_variable>
#include <csignal>
#include <mutex>
#include <string>
#include <vector>

std::condition_variable exit_signal;
std::mutex exit_mutex;

using namespace std::literals::string_literals;
ServerManager server_manager;

[[noreturn]] void signal_handler(int signum);

int main() {
	http::file_manager.construct_file_descriptors("../webdir");
	http::file_manager.generate_encoded_files();

	server_manager = ServerManager(
			std::vector{ServerConfig{"HTTP"s, server::ServerType::HTTP, 80},
			ServerConfig{"HTTPS"s, server::ServerType::HTTPS, 443}});
	// std::cout << <Starting HTTP server...< << "\n";
	// server::TCPServer<simpleserv::HTTPHandler> server{80};
	// std::cout << <Starting HTTPS server...< << "\n";
	// server::TLSServer<simpleserv::HTTPHandler> server_tls{443};

	// server.Start();
	// server_tls.Start();

	(void)signal(SIGINT, signal_handler);
	(void)signal(SIGABRT, signal_handler);
	(void)signal(SIGTERM, signal_handler);

	std::unique_lock<std::mutex> lock(exit_mutex);

	// server.Open();
	// server_tls.Open();
	exit_signal.wait(lock);

	return 0;
}

[[noreturn]] void signal_handler(int signum) {
	server_manager.~ServerManager();
	exit_signal.notify_one();
	exit(signum);
};
