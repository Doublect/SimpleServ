#ifndef WEBSERVER_SERVER_MANAGER_HPP
#define WEBSERVER_SERVER_MANAGER_HPP

#include "socket_layer/tcp_server.hpp"
#include "file_manager.hpp"
#include "server.hpp"

#include <optional>
#include <thread>
#include <vector>

struct ServerConfig {
	std::string name;
	ServerType serverType;
	std::string port;
};

struct ServerUnit {
	std::string name;
	ServerFacade server;
};

class ServerManager {
	std::vector<ServerUnit> servers;
	std::vector<std::thread> server_threads;

public:
	ServerManager() = default;

	ServerManager(ServerManager&&) = default;
	ServerManager& operator=(ServerManager&&) = default;
	ServerManager(ServerManager&) = delete;
	ServerManager& operator=(ServerManager&) = delete;

	ServerManager(std::vector<ServerConfig> configs) {
		for (auto& server : configs) {
			this->servers.push_back(ServerUnit {server.name, ServerFacade(configure_server(server))});
		}

		startup();
		open();
	}
	~ServerManager() {
		for (auto& server_unit : servers) {
			server_unit.server.close_connection();
		}

		for (auto& server_thread : server_threads) {
			server_thread.~thread();
		}
	}

	void startup() {
		for (auto& server_unit : servers) {
			std::cout << "Starting " << server_unit.name << " server..." << std::endl;
			server_unit.server.startup();
		}
	}

	void open() {
		for (auto& server_unit : servers) {
			std::cout << "Opening " << server_unit.name << " server on port nil" << std::endl;

			server_threads.push_back(server_unit.server.receive_connection());
		}
	}

private: 
	constexpr Server *configure_server(const ServerConfig& config) {
		switch (config.serverType) {
			case ServerType::HTTP:
				return new HTTPServer(config.port);
			case ServerType::HTTPS:
				return new HTTPSServer(config.port);
			default:
				throw std::runtime_error("Invalid server type");
		}
	}
};

#endif // WEBSERVER_SERVER_MANAGER_HPP