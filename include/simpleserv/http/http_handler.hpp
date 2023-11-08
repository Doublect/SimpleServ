#ifndef WEBSERVER_HTTP_HANDLER_HPP
#define WEBSERVER_HTTP_HANDLER_HPP

#include <simpleserv/http/parser.hpp>
#include <simpleserv/utility/file_manager.hpp>
#include <simpleserv/utility/logger.hpp>

#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace http {

	const std::vector<Header> capabilities { // NOLINT(cert-err58-cpp)
		Header {	"Age", "0" },
		Header { "Allow", "GET, HEAD, OPTIONS"},
		Header { "Server", "SSRV/0.1" },
	};

	static utility::FileManager file_manager{};

	class HTTPHandler {
		public:
			static std::optional<std::string> handle(const std::string& str);

		private:
			inline static HTTPResponse prepare_response(const HTTPRequest& request);
	};

	template<HTTPMethod request_method>
	class ResponseCreator {

	};

	// template<> 
	// class ResponseCreator<HTTPMethod::OPTIONS> {
	// 	const HTTPResponse ComposeResponse(const HTTPRequest& request) {
	// 		auto file_info = file_manager.get_file_headers();

	// 		if(!file_info) {
	// 			return HTTPResponse{HTTPStatusCode::INTERNAL_SERVER_ERROR, HTTP_1_1};
	// 		}

	// 		std::vector<Header> file_headers = file_info.value();
	// 	}
	// };
}; // namespace server

#endif // WEBSERVER_HTTP_HANDLER_HPP