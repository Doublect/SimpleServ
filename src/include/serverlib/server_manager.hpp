#ifndef WEBSERVER_SERVER_MANAGER_HPP
#define WEBSERVER_SERVER_MANAGER_HPP

#include "serverlib/server/tcp_server.hpp"
#include "serverlib/server/tls_server.hpp"
#include "serverlib/server/server.hpp"
#include "file_manager.hpp"

#include <cstdint>
#include <optional>
#include <thread>
#include <vector>

struct ServerConfig {
	std::string name;
	server::ServerType serverType;
	uint32_t port;
};

struct ServerUnit {
	std::string name;
	server::Server &server;
};

class ServerManager {
	std::vector<server::Server *> servers_raw;
	std::vector<std::jthread> server_threads;
	std::vector<ServerUnit> servers;

public:
	ServerManager() = default;

	ServerManager(ServerManager&&) = default;
	ServerManager& operator=(ServerManager&&) = default;
	ServerManager(ServerManager&) = delete;
	ServerManager& operator=(ServerManager&) = delete;

	ServerManager(std::vector<ServerConfig> configs) {
		for (auto& server : configs) {
			server::Server *server_ptr = configure_server(server);
			servers_raw.push_back(server_ptr);
			this->servers.push_back(ServerUnit {server.name, *server_ptr});
		}

		Start();
		Open();
	}

	~ServerManager() {
		for (auto& server_thread : server_threads) {
			server_thread.request_stop();
		}

		for (auto& server_thread : server_threads) {
			server_thread.join();
		}

		for (auto& server_unit : servers) {
			server_unit.server.Stop();
		}
	}

	void Start() {
		for (auto& server_unit : servers) {
			std::cout << "Starting " << server_unit.name << " server..." << std::endl;
			server_unit.server.Start();
		}
	}

	void Open() {
		for (auto& server_unit : servers) {
			std::cout << "Opening " << server_unit.name << " server on port " << server_unit.server.port_() << std::endl;
			
			server_threads.push_back(std::jthread([&server_unit](std::stop_token stoken) { server_unit.server.Open(stoken); }));
		}
	}

private: 
	constexpr server::Server *configure_server(const ServerConfig& config) {
		switch (config.serverType) {
			case server::ServerType::HTTP:
				return new server::TCPServer<server::HTTPHandler>(config.port);
			case server::ServerType::HTTPS:
				return new server::TLSServer<server::HTTPHandler>(config.port);
			default:
				throw std::runtime_error("Invalid server type");
		}
	}
};

#endif // WEBSERVER_SERVER_MANAGER_HPP