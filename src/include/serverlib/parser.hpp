#ifndef PARSER_HPP
#define PARSER_HPP

#include "message.hpp"
#include <expected>
#include <map>
#include <stdexcept>
#include <string>

class parser_error final : public std::exception {
public:
	parser_error() {}
	~parser_error() final {}
};

#ifndef NDEBUG
inline std::expected<std::tuple<HTTPRequest, std::string_view>, parser_error>
parse_request_line(std::string_view str);
#endif

std::expected<HTTPRequest, parser_error> parse_http_request(std::string str);
std::expected<
		std::tuple<std::map<std::string_view, std::string_view>, std::string_view>,
		parser_error>
parse_message_headers(std::string_view str);

#endif