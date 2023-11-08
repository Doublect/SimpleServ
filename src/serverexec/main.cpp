#include <simpleserv/http/http_handler.hpp>
#include <simpleserv/server/constants.hpp>
#include <simpleserv/server/server.hpp>
#include <simpleserv/server/server_manager.hpp>
#include <simpleserv/utility/file_manager.hpp>

#include <cstdlib>
#include <condition_variable>
#include <csignal>
#include <mutex>
#include <string>
#include <vector>

static std::condition_variable exit_signal;
static std::mutex exit_mutex;

using namespace std::literals::string_literals;
using namespace server;
ServerManager server_manager;

[[noreturn]] void signal_handler(int signum);

int main() {
	http::file_manager.construct_file_descriptors("../webdir");
	http::file_manager.generate_encoded_files();

	server_manager = ServerManager(
			std::vector{ServerConfig{"HTTP"s, server::ServerType::HTTP, server::HTTP_PORT},
			ServerConfig{"HTTPS"s, server::ServerType::HTTPS, server::HTTPS_PORT}});


	(void)signal(SIGINT, signal_handler);
	(void)signal(SIGABRT, signal_handler);
	(void)signal(SIGTERM, signal_handler);

	std::unique_lock<std::mutex> lock(exit_mutex);

	exit_signal.wait(lock);

	return 0;
}

[[noreturn]] void signal_handler(int signum) {
	server_manager.~ServerManager();
	exit_signal.notify_one();
	quick_exit(signum);
};