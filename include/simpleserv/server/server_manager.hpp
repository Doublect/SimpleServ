#ifndef WEBSERVER_SERVER_MANAGER_HPP
#define WEBSERVER_SERVER_MANAGER_HPP

#include <simpleserv/server/tcp_server.hpp>
#include <simpleserv/server/tls_server.hpp>
#include <simpleserv/server/server.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

namespace server {

	struct ServerConfig {
		std::string name;
		server::ServerType serverType;
		uint32_t port;
	};

	struct ServerUnit {
		std::string name;
		std::unique_ptr<Server> server;
	};

	class ServerManager {
		std::vector<std::jthread> server_threads;
		std::vector<ServerUnit> servers;

	public:
		ServerManager() = default;

		ServerManager(ServerManager&) = delete;
		ServerManager& operator=(ServerManager&) = delete;
		ServerManager(ServerManager&&) = default;
		ServerManager& operator=(ServerManager&&) = default;

		explicit ServerManager(const std::vector<ServerConfig>& configs) {
			for (const auto& server : configs) {
				std::unique_ptr<Server> server_ptr = configure_server(server);
				this->servers.push_back(ServerUnit {server.name, std::move(server_ptr)});
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
				server_unit.server->Stop();
			}
		}

		void Start() {
			for (auto& server_unit : servers) {
				std::cout << "Starting " << server_unit.name << " server...\n";
				server_unit.server->Start();
			}
		}

		void Open() {
			for (auto& server_unit : servers) {
				std::cout << "Opening " << server_unit.name << " server on port " << server_unit.server->port_() << "\n";
				
				server_threads.emplace_back([&server_unit](const std::stop_token &stoken) { server_unit.server->Open(stoken); });
			}
		}

	private: 
		constexpr static std::unique_ptr<Server> configure_server(const ServerConfig& config) {
			switch (config.serverType) {
				case server::ServerType::HTTP:
					return std::make_unique<server::TCPServer<http::HTTPHandler>>(config.port);
				case server::ServerType::HTTPS:
					return std::make_unique<server::TLSServer<http::HTTPHandler>>(config.port);
				default:
					break;
			}
			throw std::runtime_error("Invalid server type");
		}
	};

} // namespace server

#endif // WEBSERVER_SERVER_MANAGER_HPP