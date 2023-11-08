#ifndef WEBSERVER_TCP_CONNECTION_HPP
#define WEBSERVER_TCP_CONNECTION_HPP

#include <simpleserv/request_processing/connection.hpp>

#include <sys/socket.h>

#include <string>
#include <type_traits>

namespace request_processing {
	struct HTTPConnectionObject {
		int client_fd = -1;
		std::string request;
	};

	class TCPSender {
	public:
		constexpr static void SendMsg(HTTPConnectionObject &conn_obj, std::string &response) {
			send(conn_obj.client_fd, response.c_str(), response.length(), 0);
		}
	};

	class TCPCloser {
	public:
		constexpr static void CloseConnection(HTTPConnectionObject &conn_obj) {
			close(conn_obj.client_fd);
		}
	};
}; // namespace request_processing

#endif // WEBSERVER_TCP_CONNECTION_HPP