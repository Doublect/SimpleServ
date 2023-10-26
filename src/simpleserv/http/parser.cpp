#include <simpleserv/http/parser.hpp>
#include <simpleserv/utility/expected.hpp>

#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace http {

	const std::unordered_set<std::string> methods = {
			"OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};

	inline utility::expected<HTTPRequest, utility::parser_error>
	parse_request_line(utility::StringParser& sp) {
		HTTPMethod method = method_string_to_enum(sp);
		if(method == HTTPMethod::INVALID) {
			return utility::unexpected(utility::parser_error());
		}

		sp.expect_two(11, 9);
		sp.consume_all_two(11, 9);

		// TODO(Hunor): support any whitespace
		const std::string_view url = sp.next_token(' ');

		HTTPVersion version = version_string_to_enum(sp);

		if(version == HTTPVersion::HTTP_INVALID) {
			return utility::unexpected(utility::parser_error());
		}

		sp.expect("\r\n");
		
		return HTTPRequest(method, std::string(url), version);
	}

	inline utility::expected<Header, utility::parser_error>
	parse_header(std::string_view header) {
		auto [field_name, field_value] = utility::StringParser::next_token(header);

		if (field_name.ends_with(':')) {
			return utility::unexpected(utility::parser_error{});
		}
		field_name.remove_suffix(1);
	
		return Header{field_name, field_value};
	}

	std::string to_string(std::string_view sv) {
		return std::string(sv);
	}

	utility::expected<
			std::vector<Header>,
			utility::parser_error>
	parse_message_headers(std::string_view &str) {
		std::vector<Header> headers;
		std::string_view str_view(str);

		while (!str_view.starts_with("\r\n")) {
			const size_t next_header = str_view.find_first_of("\r\n");
			if (next_header == std::string::npos) {
				return utility::unexpected(utility::parser_error());
			}
			auto result = parse_header(str_view.substr(0, next_header));
			if (!result.has_value()) {
				std::cout << "Error in parsing header\n";
				break;
			}

			headers.push_back(result.value());
			str_view.remove_prefix(next_header + 2);
		}

		// Guaranteed to be \r\n
		str_view.remove_prefix(2);

		return headers;
	}

	utility::expected<HTTPRequest, utility::parser_error> parse_http_request(std::string str) {
		utility::StringParser sp(str);

		auto result = parse_request_line(sp);

		if (!result.has_value()) {
			return result;
		}

		auto request = result.value();

		auto headers_result = sp.process_function<utility::expected<std::vector<Header>, utility::parser_error>>(parse_message_headers);

		if (!headers_result.has_value()) {
			return utility::unexpected(headers_result.error());
		}

		auto headers = headers_result.value();

		for (auto &[key, value] : headers) {
			request.SetHeader(std::string(key), std::string(value));
		}
		request.SetContent(sp.process_function_own<std::string>([] (std::string_view sv) {return std::string(sv); }));

		return request;
	}

}; // namespace http