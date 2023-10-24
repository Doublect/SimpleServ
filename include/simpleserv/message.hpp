#ifndef WEBSERVER_MESSAGE_HPP
#define WEBSERVER_MESSAGE_HPP

#include <simpleserv/http.hpp>

#include <algorithm>
#include <cstring>
#include <cstdint>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

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
	return HTTPMethod::INVALID;
	throw std::runtime_error("Invalid HTTP method string: " + std::string(str));
}

enum HTTPVersion : uint8_t { HTTP_1_0, HTTP_1_1, HTTP_2_0, HTTP_INVALID };

constexpr const char *HTTP_VERSION_STRINGS[] = {"HTTP/1.0", "HTTP/1.1",
																						 "HTTP/2.0"};


#define EXPECT_TH(str, char)\
	if(*str != char || *str == '\0') {\
		throw std::runtime_error("Invalid HTTP version string");\
	}\
	str++;

constexpr HTTPVersion version_string_to_enum(const char *&str) {
	const char *end = strstr(str, "\r\n");
	if(end - str != 8) {
		throw std::runtime_error("Invalid HTTP version string");
	}

	EXPECT_TH(str, 'H');
	EXPECT_TH(str, 'T');
	EXPECT_TH(str, 'T');
	EXPECT_TH(str, 'P');
	EXPECT_TH(str, '/');
	if(str[0] == '2') {
		str++;
		EXPECT_TH(str, '.');
		EXPECT_TH(str, '0');
		return HTTP_2_0;
	} else if(str[0] == '1') {
		str++;
		EXPECT_TH(str, '.');
		if(str[0] == '0') {
			str++;
			return HTTP_1_0;
		} else if(str[0] == '1') {
			str++;
			return HTTP_1_1;
		}
	}

	return HTTPVersion::HTTP_INVALID;
}

class HTTPMessage {
protected:
	HTTPVersion version_;
	std::map<std::string, std::string> headers_;
	std::string body_;

public:
	void SetHeader(const std::string &key, std::string value) {
	headers_[key] = std::move(value);
};

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
	HTTPRequest(HTTPMethod method, std::string path, HTTPVersion version) : method_(method), path_(path) {
		this->version_ = version;
	}

	inline HTTPMethod method() const { return method_; }
	inline std::string path() const { return path_; }

	friend std::ostream &operator<<(std::ostream &os, const HTTPRequest &request) {
		os << HTTP_METHOD_STRINGS[request.method()] << " " << request.path() << " "
			<< HTTP_VERSION_STRINGS[request.version()] << "\r\n";
		for (const auto &header : request.headers()) {
			os << header.first << ": " << header.second << "\r\n";
		}
		os << "\r\n";
		os << request.body();

		return os;
	}

	std::string to_string() const {
		std::ostringstream oss;

		oss << this;

		return oss.str();
	}
};

class HTTPResponse : public HTTPMessage {
private:
	HTTPStatusCode status_code_;

public:
	HTTPResponse(HTTPStatusCode status_code, HTTPVersion version) : status_code_(status_code) {
		this->version_ = version;
		this->body_ = "";
	}

	HTTPStatusCode status_code() const { return status_code_; }

	friend std::ostream &operator<<(std::ostream &os, const HTTPResponse &response) {
		os << HTTP_VERSION_STRINGS[response.version()] << " "
				<< status_code_to_string(response.status_code()) << "\r\n";
		for (const auto &header : response.headers()) {
			os << header.first << ": " << header.second << "\r\n";
		}
		os << "\r\n";
		os << response.body();

		return os;
	}

	std::string to_string() const {
		std::ostringstream oss;

		operator<<(oss, *this);

		return oss.str();
	}
};

#endif // WEBSERVER_MESSAGE_HPP
