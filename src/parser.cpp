#include "include/serverlib/parser.hpp"

#include <cstring>
#include <iostream>
#include <unordered_set>
#include <vector>

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

constexpr const char *next_token(const char *str, const char delim = ' ') {
	while(*str != '\0' && *str != delim) {
		str++;
	}
	
	return str;
}

#define EXPECT(str, char)\
	if(*str != char || *str == '\0') {\
		return std::unexpected(parser_error());\
	}\
	str++;

inline std::expected<HTTPRequest, parser_error>
parse_request_line(const char *&str) {
	const char *cstr = str;

	HTTPMethod method = method_string_to_enum(cstr);
	if(method == HTTPMethod::INVALID) {
		return std::unexpected(parser_error());
	}

	EXPECT(cstr, ' ');

	const char *url_end = strstr(cstr, " ");

	std::string url(cstr, url_end);

	cstr = url_end + 1;

	HTTPVersion version = version_string_to_enum(cstr);

	if(version == HTTPVersion::HTTP_INVALID) {
		return std::unexpected(parser_error());
	}

	EXPECT(cstr, '\r');
	EXPECT(cstr, '\n');
	
	str = cstr;

	return HTTPRequest(method, url, version);
}

constexpr inline std::expected<Header, parser_error>
parse_header(std::string_view str) {
	const char *cstr = str.data();

	auto field_name_end = next_token(cstr);

	if (*(field_name_end-1) != ':') {
		return std::unexpected(parser_error{});
	}

	return Header{std::string_view(cstr, field_name_end - cstr - 1), std::string_view(field_name_end + 1, str.length() - (field_name_end - cstr + 1))};
}

constexpr std::expected<
		Headers,
		parser_error>
parse_message_headers(std::string_view str) {
	std::vector<Header> headers;
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

		headers.push_back(result.value());
		str_view.remove_prefix(next_header + 2);
	}

	// Guaranteed to be \r\n
	str_view.remove_prefix(2);

	return Headers{std::move(headers), std::move(str_view)};
}

std::expected<HTTPRequest, parser_error> parse_http_request(std::string str) {
	std::string_view str_view(str);

	const char *cstr = str_view.data();
	auto result = parse_request_line(cstr);

	if (!result.has_value()) {
		return std::unexpected(result.error());
	}

	auto request = result.value();

	str_view.remove_prefix(cstr - str_view.data());
	auto headers_result = parse_message_headers(str_view);

	if (!headers_result.has_value()) {
		return std::unexpected(headers_result.error());
	}

	auto [headers, restb] = headers_result.value();

	for (auto &[key, value] : headers) {
		request.SetHeader(std::string(key), std::string(value));
	}

	request.SetContent(std::string{restb});

	return request;
}