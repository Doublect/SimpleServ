#include "include/serverlib/http.hpp"

#include "include/serverlib/parser.hpp"

#include <iostream>
#include <sstream>

std::string status_code_to_string(const HTTPStatusCode& status_code) {
	switch(status_code) {
		case HTTPStatusCode::OK:
			return "200 OK";
		case HTTPStatusCode::CREATED:
			return "201 Created";
		case HTTPStatusCode::ACCEPTED:
			return "202 Accepted";
		case HTTPStatusCode::NO_CONTENT:
			return "204 No Content";
		case HTTPStatusCode::MOVED_PERMANENTLY:
			return "300 Moved Permanently";
		case HTTPStatusCode::FOUND:
			return "301 Found";
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

std::string request_to_string(const HTTPRequest& request) {
	std::ostringstream oss;
	
	oss << HTTP_METHOD_STRINGS[request.method()] << " " << request.path() << " " << HTTP_VERSION_STRINGS[request.version()] << "\r\n";
	for (auto& header : request.headers()) {        
		oss << header.first << ": " << header.second << "\r\n";
	}
	oss << "\r\n";
	oss << request.body();
	return oss.str();
}

std::string response_to_string(const HTTPResponse& response) {
	std::ostringstream oss;

	oss << HTTP_VERSION_STRINGS[response.version()] << " " << status_code_to_string(response.status_code()) << "\r\n";
	for (auto& header : response.headers()) {
		oss << header.first << ": " << header.second << "\r\n";
	}
	oss << "\r\n";
	oss << response.body();
	return oss.str();
}