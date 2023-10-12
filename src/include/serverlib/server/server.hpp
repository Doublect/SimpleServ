#ifndef WEBSERVER_SERVER_HPP
#define WEBSERVER_SERVER_HPP

#include <thread>

namespace server {
	class Server {
	public:
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Open(std::stop_token stoken) = 0;

		int port_() { return _port; }

	protected:
		int _port;
	};

	enum class ServerType {
		HTTP,
		HTTPS
	};

}; // namespace server

#endif // WEBSERVER_SERVER_HPP