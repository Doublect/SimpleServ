#include <simpleserv/http/http_handler.hpp>

#include <simpleserv/http/http.hpp>
#include <simpleserv/http/parser.hpp>
#include <simpleserv/http/message.hpp>
#include <simpleserv/utility/file_manager.hpp>
#include <simpleserv/utility/logger.hpp>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>

namespace http {

	constexpr std::string remove_query(std::string str) {
		const size_t query_index = str.find('?');
		if (query_index != std::string::npos) {
			return str.substr(0, query_index);
		}

		return str;
	}

	std::filesystem::path convert_to_filedirectory(const std::string& request_path) {
		return {"../webdir" + remove_query(request_path)};
	}

	std::optional<std::string> HTTPHandler::handle(const std::string& str) {
		auto result = parse_http_request(str);

		if (!result.has_value()) {
			utility::Logger::warning("An error has occured during the parsing of an HTTP request");
			utility::Logger::warning(result.error().what());

			return std::nullopt;
		}

		auto request = result.value();

		return prepare_response(request).to_string();
	}

	inline HTTPResponse HTTPHandler::prepare_response(const HTTPRequest& request) {
		HTTPResponse response =
				HTTPResponse(HTTPStatusCode::OK, HTTPVersion::HTTP_1_1);

		const std::filesystem::path path = convert_to_filedirectory(request.path());
		//std::cout << "Requested path: " << path << "\n";
		auto expected_file_data = file_manager.get_file(path);

		if(!expected_file_data.has_value()) {
			utility::Logger::warning(expected_file_data.error().what());
			response = HTTPResponse(HTTPStatusCode::NOT_FOUND, HTTPVersion::HTTP_1_1);
			return response;
		}

		auto file_data = expected_file_data.value();

		response.SetHeader("Content-Type", decode_filetype(file_data.filetype));
		if(file_data.file_encoding != utility::FileEncoding::NONE) {
			response.SetHeader("Content-Encoding", decode_file_encoding(file_data.file_encoding));
		}

		if(request.method() != HTTPMethod::HEAD) {
			response.SetContent(file_data.content);
		}

		return response;
	}
}; // namespace server