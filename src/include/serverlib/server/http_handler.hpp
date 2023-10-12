#ifndef WEBSERVER_HTTP_HANDLER_HPP
#define WEBSERVER_HTTP_HANDLER_HPP

#include "serverlib/file_manager.hpp"
#include "serverlib/http/parser.hpp"

#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace server {

	static FileManager file_manager = FileManager();

	template <typename T>
	concept IHandler = requires(T handler, std::string str)
	{
		{ handler.handle(str) } -> std::same_as<std::optional<std::string>>;
	};

	constexpr std::string remove_query(std::string str) {
		size_t query_index = str.find("?");
		if (query_index != std::string::npos) {
			return str.substr(0, query_index);
		}

		return str;
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

				std::filesystem::path path = "../webdir" + remove_query(request.path());
				//std::cout << "Requested path: " << path << std::endl;
				auto expected_file_data = file_manager.get_file(std::filesystem::path(path));

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
				response.SetContent(file_data.content);

				return response;
			}
	};
} // namespace server

#endif // WEBSERVER_HTTP_HANDLER_HPP