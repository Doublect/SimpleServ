#ifndef SIMPLESERV_PARSER_HPP
#define SIMPLESERV_PARSER_HPP

#include <simpleserv/http/message.hpp>
#include <simpleserv/utility/expected.hpp>
#include <simpleserv/utility/string_parser.hpp>

#include <format>
#include <map>
#include <source_location>
#include <stdexcept>
#include <string>
#include <vector>

namespace http {
	struct Header {
		std::string_view key;
		std::string_view value;
	};

	utility::expected<HTTPRequest, utility::parser_error> parse_http_request(std::string str);

}; //namespace http
#endif