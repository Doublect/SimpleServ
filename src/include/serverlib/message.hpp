//
// Created by doublect on 7/27/23.
//

#ifndef WEBSERVER_MESSAGE_HPP
#define WEBSERVER_MESSAGE_HPP

#include <algorithm>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>

enum HTTPMethod { GET, POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT };

enum HTTPStatusCode {
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 204,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503
};

static std::string HTTP_METHOD_STRINGS[] = {
		"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT"};
#define OLD
#ifdef OLD
constexpr HTTPMethod method_string_to_enum(std::string_view str) {
	std::string string_upper;
	std::transform(str.begin(), str.end(), std::back_inserter(string_upper),
								 [](char c) { return toupper(c); });

	for (int i = 0; i < 8; i++) {
		if (string_upper == HTTP_METHOD_STRINGS[i]) {
			return static_cast<HTTPMethod>(i);
		}
	}

	throw std::runtime_error("Invalid HTTP method string: " + std::string(str));
}
#else

#define EXPECT_SW(str, char)\
	if(*str != char || *str == '\0') {\
		break;\
	}\
	str++;
constexpr HTTPMethod method_string_to_enum(const char *&str) {
	switch (*str) {
		case 'G':
			str++;
			EXPECT_SW(str, 'E');
			EXPECT_SW(str, 'T');
			return HTTPMethod::GET;
		case 'P':
			str++;
			if(*str == 'O') {
				str++;
				EXPECT_SW(str, 'S');
				EXPECT_SW(str, 'T');
				return HTTPMethod::POST;
			} else if(*str == 'U') {
				str++;
				EXPECT_SW(str, 'T');
				return HTTPMethod::PUT;
			}
			break;
		case 'D':
			str++;
			EXPECT_SW(str, 'E');
			EXPECT_SW(str, 'L');
			EXPECT_SW(str, 'E');
			EXPECT_SW(str, 'T');
			EXPECT_SW(str, 'E');
			return HTTPMethod::DELETE;
		case 'H':
			str++;
			EXPECT_SW(str, 'E');
			EXPECT_SW(str, 'A');
			EXPECT_SW(str, 'D');
			return HTTPMethod::HEAD;
		case 'O':
			str++;
			EXPECT_SW(str, 'P');
			EXPECT_SW(str, 'T');
			EXPECT_SW(str, 'I');
			EXPECT_SW(str, 'O');
			EXPECT_SW(str, 'N');
			EXPECT_SW(str, 'S');
			return HTTPMethod::OPTIONS;
		case 'T':
			str++;
			EXPECT_SW(str, 'R');
			EXPECT_SW(str, 'A');
			EXPECT_SW(str, 'C');
			EXPECT_SW(str, 'E');
			return HTTPMethod::TRACE;
		case 'C':
			str++;
			EXPECT_SW(str, 'O');
			EXPECT_SW(str, 'N');
			EXPECT_SW(str, 'N');
			EXPECT_SW(str, 'E');
			EXPECT_SW(str, 'C');
			EXPECT_SW(str, 'T');
			return HTTPMethod::CONNECT;

		default:
			break;
	}

	throw std::runtime_error("Invalid HTTP method string: " + std::string(str));
}
#endif

enum HTTPVersion { HTTP_1_0, HTTP_1_1, HTTP_2_0 };

static std::string HTTP_VERSION_STRINGS[] = {"HTTP/1.0", "HTTP/1.1",
																						 "HTTP/2.0"};

#ifdef OLD
constexpr HTTPVersion version_string_to_enum(std::string_view str) {
	std::string string_upper;
	std::transform(str.begin(), str.end(), std::back_inserter(string_upper),
								 [](char c) { return toupper(c); });

	if (string_upper == "HTTP/1.0") {
		return HTTP_1_0;
	} else if (string_upper == "HTTP/1.1") {
		return HTTP_1_1;
	} else if (string_upper == "HTTP/2.0") {
		return HTTP_2_0;
	}

	throw std::runtime_error("Invalid HTTP version string: " + string_upper);
}
#else
// constexpr HTTPVersion version_string_to_enum(std::string_view str) {
// 	if(str.starts_with("HTTP/") && str.length() == 8) {
// 		if (str.ends_with("1.0")) {
// 			return HTTP_1_0;
// 		} else if (str.ends_with("1.1")) {
// 			return HTTP_1_1;
// 		} else if (str.ends_with("2.0")) {
// 			return HTTP_2_0;
// 		}
// 	}

// 	throw std::runtime_error("Invalid HTTP version string: " + std::string(str));
// }

#define EXPECT(str, char)\
	if(*str != char || *str == '\0') {\
		throw std::runtime_error("Invalid HTTP version string");\
	}\
	str++;

constexpr HTTPVersion version_string_to_enum(const char *&str) {
	const char *end = strstr(str, "\r\n");
	if(end - str != 8) {
		throw std::runtime_error("Invalid HTTP version string");
	}

	EXPECT(str, 'H');
	EXPECT(str, 'T');
	EXPECT(str, 'T');
	EXPECT(str, 'P');
	EXPECT(str, '/');
	if(str[0] == '2') {
		str++;
		EXPECT(str, '.');
		EXPECT(str, '0');
		return HTTP_2_0;
	} else if(str[0] == '1') {
		str++;
		EXPECT(str, '.');
		if(str[0] == '0') {
			str++;
			return HTTP_1_0;
		} else if(str[0] == '1') {
			str++;
			return HTTP_1_1;
		}
	}
}

#endif
class HTTPMessage {
protected:
	HTTPVersion version_;
	std::map<std::string, std::string> headers_;
	std::string body_;

public:
	void SetHeader(std::string key, std::string value);

	void SetContent(std::string content) { body_ = content; }

	HTTPVersion version() const { return version_; }
	std::map<std::string, std::string> headers() const { return headers_; }
	std::string body() const { return body_; }
};

class HTTPRequest : public HTTPMessage {
private:
	HTTPMethod method_;
	std::string path_;

public:
	HTTPRequest(HTTPMethod method, std::string path, HTTPVersion version) {
		this->method_ = method;
		this->path_ = path;
		this->version_ = version;
	}

	HTTPMethod method() const { return method_; }
	std::string path() const { return path_; }
};

class HTTPResponse : public HTTPMessage {
private:
	HTTPStatusCode status_code_;

public:
	HTTPResponse(HTTPStatusCode status_code, HTTPVersion version) {
		this->status_code_ = status_code;
		this->version_ = version;
		this->body_ = "";
	}

	HTTPStatusCode status_code() const { return status_code_; }
};

#endif // WEBSERVER_MESSAGE_HPP
