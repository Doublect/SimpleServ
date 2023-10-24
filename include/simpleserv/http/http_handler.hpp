#ifndef WEBSERVER_HTTP_HANDLER_HPP
#define WEBSERVER_HTTP_HANDLER_HPP

#include <simpleserv/file_manager.hpp>
#include <simpleserv/http/parser.hpp>

#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace http {

	const std::vector<Header> capabilities {
		Header {	"Age", "0" },
		Header { "Allow", "GET, HEAD, OPTIONS"},
		Header { "Server", "SSRV/0.1" },
	};

	static FileManager file_manager = FileManager();

	constexpr std::string remove_query(std::string str) {
		size_t query_index = str.find("?");
		if (query_index != std::string::npos) {
			return str.substr(0, query_index);
		}

		return str;
	}

	const std::filesystem::path convert_to_filedirectory(const std::string& request_path) {
		return std::filesystem::path("../webdir" + remove_query(request_path));
	}

	class HTTPHandler {
		public:
			static std::optional<std::string> handle(std::string str) {
				auto result = parse_http_request(str);

				if (!result.has_value()) {
					std::cout << "Error parsing HTTP request" << std::endl;

					return std::nullopt;
				}

				auto request = result.value();

				return prepare_response(request).to_string();
			}

		private:
			inline static HTTPResponse prepare_response(HTTPRequest request) {
				HTTPResponse response =
						HTTPResponse(HTTPStatusCode::OK, HTTPVersion::HTTP_1_1);

				std::filesystem::path path = convert_to_filedirectory(request.path());
				//std::cout << "Requested path: " << path << std::endl;
				auto expected_file_data = file_manager.get_file(path);

				if(!expected_file_data.has_value()) {
					std::cerr << expected_file_data.error().what() << std::endl;
					response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTPVersion::HTTP_1_1);
					return response;
				}

				auto file_data = expected_file_data.value();

				response.SetHeader("Content-Type", decode_filetype(file_data.filetype));
				if(file_data.file_encoding != FileEncoding::NONE) {
					response.SetHeader("Content-Encoding", decode_file_encoding(file_data.file_encoding));
				}

				if(request.method() != HTTPMethod::HEAD) {
					response.SetContent(file_data.content);
				}

				return response;
			}
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