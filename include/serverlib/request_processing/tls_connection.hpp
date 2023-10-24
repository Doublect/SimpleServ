#ifndef WEBSERVER_TLS_CONNECTION_HPP
#define WEBSERVER_TLS_CONNECTION_HPP

#include <serverlib/request_processing/connection.hpp>

#include <wolfssl/ssl.h>

#include <string>
#include <type_traits>

namespace request_processing {
	struct HTTPSConnectionObject {
		int client_fd;
		WOLFSSL* ssl;
		std::string request;
	};

	class TLSSender {
	public:
		constexpr static void SendMsg(HTTPSConnectionObject &co, std::string &response) {
			int err, ret;
			do {
				ret = wolfSSL_write(co.ssl, response.c_str(),
														static_cast<int>(response.length()));
				err = wolfSSL_get_error(co.ssl, ret);
			} while (err == WOLFSSL_ERROR_WANT_WRITE);
		}
	};

	class TLSCloser {
	public:
		constexpr static void CloseConnection(HTTPSConnectionObject &co) {
			close(co.client_fd);
			wolfSSL_shutdown(co.ssl);
			wolfSSL_free(co.ssl); /* Free the wolfSSL object              */
		}
	};
}; // namespace request_processing

#endif // WEBSERVER_TLS_CONNECTION_HPP