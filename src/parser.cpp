#include "include/serverlib/parser.hpp"

#include <cstring>
#include <iostream>
#include <unordered_set>

std::unordered_set<std::string> methods = {
		"OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT"};

inline std::tuple<std::string_view, std::string_view>
next_token(std::string_view str, size_t start = 0, std::string delim = " ") {
	size_t found = str.find(delim, start);
	if (found == std::string::npos) {
		return std::make_tuple(str.substr(start, str.length() - start), "");
	}

	std::string_view tok = str.substr(start, found - start);
	str.remove_prefix(found + delim.length());
	return std::make_tuple(tok, str);
}

inline const char *next_token(const char *str, std::string delim = " ") {
	while(*str != '\0' && *str == ' ') {
		str++;
	}
	
	return str;
}
#define OLD
#ifdef OLD
inline std::expected<std::tuple<HTTPRequest, std::string_view>, parser_error>
parse_request_line(std::string_view str) {
	//std::cout << "parse_request_line: " << str << std::endl;	
	const auto [method_str, resta] = next_token(str);
	if (!methods.contains(std::string(method_str))) {
		return std::unexpected(parser_error());
	}

	HTTPMethod method = method_string_to_enum(method_str);

	const auto [url, restb] = next_token(resta);

	const auto [version_str, restc] = next_token(restb, 0, "\r\n");

	HTTPVersion version = version_string_to_enum(version_str);

	return std::make_tuple(HTTPRequest(method, std::string(url), version), restc);
}
#else
#define EXPECT(str, char)\
	if(*str != char || *str == '\0') {\
		return std::unexpected(parser_error());\
	}\
	str++;

inline std::expected<std::tuple<HTTPRequest, std::string_view>, parser_error>
parse_request_line(std::string_view str) {
	const char *cstr = str.data();
	//std::cout << "parse_request_line: " << str << std::endl;	
	const auto [method_str, resta] = next_token(str);
	if (!methods.contains(std::string(method_str))) {
		return std::unexpected(parser_error());
	}

	HTTPMethod method = method_string_to_enum(cstr);

	EXPECT(cstr, ' ');

	const char * url_end = strstr(cstr, " ");

	std::string url(cstr, url_end);

	cstr = url_end + 1;

	HTTPVersion version = version_string_to_enum(cstr);

	str.remove_prefix(cstr - str.data());

	return std::make_tuple(HTTPRequest(method, url, version), str);
}
#endif

inline std::expected<std::tuple<std::string_view, std::string_view>, parser_error>
parse_header(std::string_view str) {
	auto [field_name, rest] = next_token(str, 0);

	if (!field_name.ends_with(':')) {
		return std::unexpected(parser_error{});
	}

	field_name.remove_suffix(1);

	return std::make_tuple(field_name, rest);
}

std::expected<
		std::tuple<std::map<std::string_view, std::string_view>, std::string_view>,
		parser_error>
parse_message_headers(std::string_view str) {
	std::map<std::string_view, std::string_view> headers;
	std::string_view str_view(str);

	while (!str_view.starts_with("\r\n")) {
		size_t next_header = str_view.find_first_of("\r\n");
		if (next_header == std::string::npos) {
			return std::unexpected(parser_error());
		}
		auto result = parse_header(str_view.substr(0, next_header));
		if (!result.has_value()) {
			std::cout << "Error in parsing header" << std::endl;
			break;
		}
		auto [key, value] = result.value();

		headers[key] = value;
		str_view.remove_prefix(next_header + 2);
	}

	return std::make_tuple<std::map<std::string_view, std::string_view>,
												 std::string_view>(std::move(headers),
																					 std::move(str_view));
}

std::expected<HTTPRequest, parser_error> parse_http_request(std::string str) {
	std::string_view str_view(str);
	auto result = parse_request_line(str_view);

	if (!result.has_value()) {
		return std::unexpected(result.error());
	}

	auto [request, rest] = result.value();

	//std::cout << "B" << rest << "E" << std::endl;

	auto headers_result = parse_message_headers(rest);

	if (!headers_result.has_value()) {
		return std::unexpected(headers_result.error());
	}

	//std::cout << "Parsed headers" << std::endl;

	auto [headers, restb] = headers_result.value();

	for (auto &[key, value] : headers) {
		request.SetHeader(std::string(key), std::string(value));
	}

	request.SetContent(std::string{restb});

	return request;
}