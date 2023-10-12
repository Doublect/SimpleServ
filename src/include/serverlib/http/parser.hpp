#ifndef PARSER_HPP
#define PARSER_HPP

#include "message.hpp"

#include <expected>
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

	std::expected<HTTPRequest, parser_error> parse_http_request(std::string str);

}; //namespace http
#endif