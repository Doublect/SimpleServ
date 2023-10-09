#ifndef HTTP_HPP
#define HTTP_HPP

#include <algorithm>
#include <expected>
#include <iostream>
#include <map>
#include <string>

enum HTTPMethod { GET, POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT, INVALID };

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

static std::string HTTP_RESPONSE_STATUS[] = {
	"200 OK",
	"201 Created",
	"202 Accepted",
	"204 No Content",
	"301 Moved Permanently",
	"302 Found",
	"304 Not Modified",
	"400 Bad Request",
	"401 Unauthorized",
	"403 Forbidden",
	"404 Not Found",
	"500 Internal Server Error",
	"501 Not Implemented",
	"502 Bad Gateway",
	"503 Service Unavailable"
};

constexpr std::string status_code_to_string(const HTTPStatusCode &status_code) {
	switch (status_code) {
	case HTTPStatusCode::CONTINUE:
		return "100 Continue";
	case HTTPStatusCode::SWITCHING_PROTOCOLS:
		return "101 Switching Protocols";
	case HTTPStatusCode::OK:
		return "200 OK";
	case HTTPStatusCode::CREATED:
		return "201 Created";
	case HTTPStatusCode::ACCEPTED:
		return "202 Accepted";
	case HTTPStatusCode::NO_CONTENT:
		return "204 No Content";
	case HTTPStatusCode::MULTIPLE_CHOICES:
		return "300 Multiple Choices";
	case HTTPStatusCode::MOVED_PERMANENTLY:
		return "300 Moved Permanently";
	case HTTPStatusCode::FOUND:
		return "301 Found";
	case HTTPStatusCode::SEE_OTHER:
		return "303 See Other";
	case HTTPStatusCode::NOT_MODIFIED:
		return "304 Not Modified";
	case HTTPStatusCode::BAD_REQUEST:
		return "400 Bad Request";
	case HTTPStatusCode::UNAUTHORIZED:
		return "401 Unauthorized";
	case HTTPStatusCode::FORBIDDEN:
		return "403 Forbidden";
	case HTTPStatusCode::NOT_FOUND:
		return "404 Not Found";
	case HTTPStatusCode::INTERNAL_SERVER_ERROR:
		return "500 Internal Server Error";
	case HTTPStatusCode::NOT_IMPLEMENTED:
		return "501 Not Implemented";
	case HTTPStatusCode::BAD_GATEWAY:
		return "502 Bad Gateway";
	case HTTPStatusCode::SERVICE_UNAVAILABLE:
		return "503 Service Unavailable";
	default:
		return "500 Internal Server Error";
	}
}

#endif