#include "include/serverlib/http.hpp"

#include <iostream>
#include <sstream>

constexpr HTTPMethod method_string_to_enum(std::string& str) {
    //std::transform(str.begin(), str.end(), std::back_inserter(str), [](char c) { return toupper(c); });

    for (int i = 0; i < 8; i++) {
        if (str == HTTP_METHOD_STRINGS[i]) {
            return (HTTPMethod) i;
        }
    }

    throw std::runtime_error("Invalid HTTP method string: " + str);
}

constexpr HTTPVersion version_string_to_enum(std::string_view str) {
    std::string string_upper;
    std::transform(str.begin(), str.end(),
                std::back_inserter(string_upper),
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

//TODO: Implement header support
HTTPRequest string_to_request(std::string& str) {
    int method_end = str.find(" ");
    std::string method_str = str.substr(0, method_end);

    int uri_end = str.find(" ", method_end + 1);
    std::string path = str.substr(method_end + 1, uri_end - method_end - 1);

    int version_end = str.find("\r\n", uri_end + 1);
    std::string version_str = str.substr(uri_end + 1, version_end - uri_end - 1);

    HTTPRequest request {
        method_string_to_enum(method_str),
        path,
        version_string_to_enum(version_str)
    };

    size_t header_ptr = version_end;
    size_t found = str.find("\r\n", header_ptr + 1);
    while(found != header_ptr + 2 && found != std::string::npos) {
        int key_end = str.find(":", header_ptr);
        if(key_end == std::string::npos) {
            break;
        }

        std::string key = str.substr(header_ptr, key_end - header_ptr);
        int value_end = str.find("\r\n", key_end);
        std::string value = str.substr(key_end + 1, value_end - key_end - 1);
        request.SetHeader(key, value);
        header_ptr = found;

        found = str.find("\r\n", header_ptr + 1);
    }

    std::string body = str.substr(header_ptr + 4);
    std::cout << "Body: " << body << std::endl;
    request.SetContent(body);

    return request;
}