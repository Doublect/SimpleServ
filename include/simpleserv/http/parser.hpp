#ifndef SIMPLESERV_PARSER_HPP
#define SIMPLESERV_PARSER_HPP

#include <simpleserv/http/message.hpp>
#include <simpleserv/utility/expected.hpp>

#include <string>
#include <string_view>

namespace http {
	struct Header {
		std::string_view key;
		std::string_view value;
	};

	utility::expected<HTTPRequest, utility::parser_error> parse_http_request(const std::string& str);

}; //namespace http
#endif