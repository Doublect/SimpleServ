#ifndef SIMPLESERV_PARSER_HPP
#define SIMPLESERV_PARSER_HPP

#include <simpleserv/message.hpp>
#include <simpleserv/utility/expected.hpp>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace http {

	class parser_error final : public std::exception {
	public:
		parser_error() {}
		~parser_error() final {}
	};

	struct Header {
		std::string_view key;
		std::string_view value;
	};

	struct Headers {
		std::vector<Header> headers;
		std::string_view rest;
	};

	utility::expected<HTTPRequest, parser_error> parse_http_request(std::string str);

}; //namespace http
#endif