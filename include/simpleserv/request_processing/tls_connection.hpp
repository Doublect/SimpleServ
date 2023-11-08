#ifndef WEBSERVER_TLS_CONNECTION_HPP
#define WEBSERVER_TLS_CONNECTION_HPP

#include <simpleserv/request_processing/connection.hpp>

#include <wolfssl/ssl.h>

#include <string>
#include <type_traits>

namespace request_processing {
	struct HTTPSConnectionObject {
		HTTPSConnectionObject() = default;
		HTTPSConnectionObject(int fd, WOLFSSL* ssl_in, std::string&& request_in) : client_fd(fd), ssl(ssl_in), request(request_in) {}

		int client_fd = -1;
		WOLFSSL* ssl{};
		std::string request;
	};

	class TLSSender {
	public:
		constexpr static void SendMsg(HTTPSConnectionObject &conn_obj, std::string &response) {
			int err = WOLFSSL_ERROR_WANT_WRITE;
			int ret = 0;
			while (err == WOLFSSL_ERROR_WANT_WRITE) {
				ret = wolfSSL_write(conn_obj.ssl, response.c_str(),
														static_cast<int>(response.length()));
				err = wolfSSL_get_error(conn_obj.ssl, ret);
			}
		}
	};

	class TLSCloser {
	public:
		constexpr static void CloseConnection(HTTPSConnectionObject &conn_obj) {
			close(conn_obj.client_fd);
			wolfSSL_shutdown(conn_obj.ssl);
			wolfSSL_free(conn_obj.ssl); /* Free the wolfSSL object              */
		}
	};
}; // namespace request_processing

#endif // WEBSERVER_TLS_CONNECTION_HPP