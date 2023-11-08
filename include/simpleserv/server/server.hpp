#ifndef WEBSERVER_SERVER_HPP
#define WEBSERVER_SERVER_HPP

#include <thread>

namespace server {
	class Server {
	public:
		Server() = delete;

		Server(const Server&) = delete;
		Server& operator=(const Server&) = delete;
		Server(Server&&) = delete;
		Server& operator=(Server&&) = delete;


		virtual ~Server() = default;

		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Open(std::stop_token stoken) = 0;

		[[nodiscard]] uint32_t port_() const { return _port; }

	protected:
		explicit Server(uint32_t port) : _port(port) {}

		uint32_t _port;
	};

	enum class ServerType {
		HTTP,
		HTTPS
	};

}; // namespace server

#endif // WEBSERVER_SERVER_HPP