#ifndef HTTP_HPP
#define HTTP_HPP

#include "message.hpp"

#include <algorithm>
#include <expected>
#include <iostream>
#include <string>
#include <map>

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


std::string request_to_string(const HTTPRequest& request);
std::string response_to_string(const HTTPResponse& response);

#endif